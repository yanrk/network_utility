/********************************************************
 * Description : enet client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#include "enet_client_impl.h"
#include "base.h"

EnetClientImpl::EnetClientImpl(const void * user_data)
    : m_running(false)
    , m_user_data(user_data)
    , m_sink(nullptr)
    , m_host()
    , m_port(0)
    , m_enet_host(nullptr)
    , m_enet_peer(nullptr)
    , m_send_data_list()
    , m_send_data_mutex()
    , m_send_data_thread()
    , m_event_list()
    , m_event_mutex()
    , m_event_condition()
    , m_event_thread()
    , m_callback_list()
    , m_callback_mutex()
    , m_callback_condition()
    , m_callback_thread()
{

}

EnetClientImpl::~EnetClientImpl()
{
    exit();
}

bool EnetClientImpl::init(EnetClientSink * sink, const char * host, uint16_t port)
{
    exit();

    RUN_LOG_DBG("enet client init begin");

    if (nullptr == host || 0 == port)
    {
        RUN_LOG_ERR("enet client init failure while invalid parameters");
        return false;
    }

    if (enet_initialize() < 0)
    {
        RUN_LOG_ERR("enet client init failure while enet initialize failed");
        return false;
    }

    ENetHost * enet_host = enet_host_create(nullptr, 1, 1, 0, 0);
    if (nullptr == enet_host)
    {
        RUN_LOG_ERR("enet client init failure while create enet host failed");
        return false;
    }

    m_running = true;
    m_sink = sink;
    m_host = host;
    m_port = port;
    m_enet_host = enet_host;

    m_event_thread = std::thread([this]{
        while (m_running)
        {
            std::list<bool> event_list;

            {
                std::unique_lock<std::mutex> locker(m_event_mutex);
                while (m_running && m_event_list.empty())
                {
                    m_event_condition.wait(locker);
                }
                event_list.swap(m_event_list);
            }

            for (std::list<bool>::const_iterator iter = event_list.begin(); event_list.end() != iter && m_running; ++iter)
            {
                if (*iter)
                {
                    do_connect();
                }
                else
                {
                    do_close();
                }
            }
        }
    });

    m_callback_thread = std::thread([this]{
        while (m_running)
        {
            std::list<bool> callback_list;

            {
                std::unique_lock<std::mutex> locker(m_callback_mutex);
                while (m_running && m_callback_list.empty())
                {
                    m_callback_condition.wait(locker);
                }
                callback_list.swap(m_callback_list);
            }

            for (std::list<bool>::const_iterator iter = callback_list.begin(); callback_list.end() != iter && m_running; ++iter)
            {
                if (nullptr == m_sink)
                {
                    continue;
                }
                if (*iter)
                {
                    m_sink->on_enet_connect(m_user_data);
                }
                else
                {
                    m_sink->on_enet_close(m_user_data);
                }
            }
        }
    });

    do
    {
        if (!m_event_thread.joinable())
        {
            RUN_LOG_ERR("enet client init failure while create event thread failed");
            break;
        }

        if (!m_callback_thread.joinable())
        {
            RUN_LOG_ERR("enet client init failure while create callback thread failed");
            break;
        }

        RUN_LOG_DBG("enet client init success");

        return true;
    } while (false);

    RUN_LOG_ERR("enet client init failure");

    exit();

    return false;
}

void EnetClientImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("enet client exit begin");

        m_running = false;

        do_close();

        if (m_event_thread.joinable())
        {
            m_event_condition.notify_one();
            m_event_thread.join();
        }

        if (m_callback_thread.joinable())
        {
            m_callback_condition.notify_one();
            m_callback_thread.join();
        }

        m_event_list.clear();
        m_callback_list.clear();

        if (nullptr != m_enet_host)
        {
            enet_host_destroy(m_enet_host);
            m_enet_host = nullptr;
        }

        m_enet_peer = nullptr;

        RUN_LOG_DBG("enet client exit end");
    }
}

void EnetClientImpl::connect()
{
    {
        std::lock_guard<std::mutex> locker(m_event_mutex);
        m_event_list.push_back(true);
    }
    m_event_condition.notify_one();
}

void EnetClientImpl::close()
{
    {
        std::lock_guard<std::mutex> locker(m_event_mutex);
        m_event_list.push_back(false);
    }
    m_event_condition.notify_one();
}

void EnetClientImpl::on_connect()
{
    {
        std::lock_guard<std::mutex> locker(m_callback_mutex);
        m_callback_list.push_back(true);
    }
    m_callback_condition.notify_one();
}

void EnetClientImpl::on_close()
{
    {
        std::lock_guard<std::mutex> locker(m_callback_mutex);
        m_callback_list.push_back(false);
    }
    m_callback_condition.notify_one();
}

void EnetClientImpl::do_connect()
{
    if (!m_running)
    {
        on_close();
        return;
    }

    if (nullptr != m_enet_peer && ENetPeerState::ENET_PEER_STATE_DISCONNECTED != m_enet_peer->state)
    {
        enet_peer_reset(m_enet_peer);
    }

    if (m_send_data_thread.joinable())
    {
        m_send_data_thread.join();
    }

    m_enet_peer = nullptr;
    m_send_data_list.clear();

    ENetAddress address;
    enet_address_set_host(&address, m_host.c_str());
    address.port = m_port;

    ENetPeer * enet_peer = enet_host_connect(m_enet_host, &address, 1, 0);
    if (nullptr == enet_peer)
    {
        if (nullptr != m_sink)
        {
            m_sink->on_enet_error(m_user_data, "connect", "unable to resolve address");
        }
        on_close();
        return;
    }

    ENetEvent event;
    if (enet_host_service(m_enet_host, &event, 5000) > 0 && ENET_EVENT_TYPE_CONNECT == event.type)
    {
        m_enet_peer = enet_peer;
        on_connect();
    }
    else
    {
        enet_peer_reset(enet_peer);
        on_close();
        return;
    }

    m_send_data_thread = std::thread([this]{
        ENetEvent event;
        while (true)
        {
            std::list<std::string> send_data_list;

            {
                std::lock_guard<std::mutex> locker(m_send_data_mutex);
                send_data_list.swap(m_send_data_list);
            }

            for (std::list<std::string>::const_iterator iter = send_data_list.begin(); send_data_list.end() != iter; ++iter)
            {
                const std::string & data = *iter;

                ENetPacket * packet = enet_packet_create(data.data(), data.size(), ENET_PACKET_FLAG_RELIABLE);
                if (nullptr != packet && enet_peer_send(m_enet_peer, 0, packet) < 0)
                {
                    enet_packet_destroy(packet);
                }
            }

            if (enet_host_service(m_enet_host, &event, 1) > 0)
            {
                switch (event.type)
                {
                    case ENET_EVENT_TYPE_RECEIVE:
                    {
                        if (nullptr != m_sink)
                        {
                            m_sink->on_enet_recv(m_user_data, event.packet->data, static_cast<uint32_t>(event.packet->dataLength));
                        }
                        enet_packet_destroy(event.packet);
                        break;
                    }
                    case ENET_EVENT_TYPE_DISCONNECT:
                    {
                        on_close();
                        return;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
            else if (!is_connected())
            {
                return;
            }
        }
    });
}

void EnetClientImpl::do_close()
{
    if (is_connected())
    {
        if (nullptr != m_enet_peer)
        {
            enet_peer_disconnect(m_enet_peer, 0);
        }

        if (m_send_data_thread.joinable())
        {
            m_send_data_thread.join();
        }

        m_enet_peer = nullptr;

        on_close();
    }
}

bool EnetClientImpl::send_message(std::string && packet)
{
    if (!is_connected())
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> locker(m_send_data_mutex);
        m_send_data_list.emplace_back(std::move(packet));
    }

    return true;
}

bool EnetClientImpl::is_connected() const
{
    return nullptr != m_enet_peer && ENetPeerState::ENET_PEER_STATE_CONNECTED == m_enet_peer->state;
}

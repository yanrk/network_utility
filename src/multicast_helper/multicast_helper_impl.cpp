/********************************************************
 * Description : multicast helper implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include "multicast_helper_impl.h"
#include "base.h"

MulticastHelperImpl::MulticastHelperImpl()
    : m_running(false)
    , m_protocol_prefix()
    , m_protocol_suffix()
    , m_multicast_recver(nullptr)
    , m_multicast_group_host()
    , m_multicast_group_port(0)
    , m_multicast_endpoint()
    , m_peer_endpoint()
    , m_send_context()
    , m_send_socket(m_send_context)
    , m_recv_context()
    , m_recv_socket(m_recv_context)
    , m_send_data_list()
    , m_recv_buffer()
    , m_recv_data_list()
    , m_recv_data_mutex()
    , m_recv_data_condition()
    , m_send_run_thread()
    , m_recv_run_thread()
    , m_recv_handle_thread()
{

}

MulticastHelperImpl::~MulticastHelperImpl()
{
    exit();
}

bool MulticastHelperImpl::init(MulticastRecver * multicast_recver, const std::string & multicast_group_host, uint16_t multicast_group_port, const std::string & protocol_prefix, const std::string & protocol_suffix)
{
    exit();

    do
    {
        RUN_LOG_DBG("multicast helper init begin");

        if (multicast_group_host.empty())
        {
            RUN_LOG_ERR("multicast helper init failure while multicast group host is empty");
            break;
        }

        if (0 == multicast_group_port)
        {
            RUN_LOG_ERR("multicast helper init failure while multicast group port is zero");
            break;
        }

        m_running = true;

        m_protocol_prefix = protocol_prefix;
        m_protocol_suffix = protocol_suffix;
        m_multicast_recver = multicast_recver;
        m_multicast_group_host = multicast_group_host;
        m_multicast_group_port = multicast_group_port;
        m_multicast_endpoint = asio::ip::udp::endpoint(asio::ip::address::from_string(multicast_group_host), multicast_group_port);

        std::error_code err;
        m_send_socket.open(asio::ip::udp::v4(), err);
        if (err)
        {
            RUN_LOG_ERR("multicast helper init failure while send socket open error (%s)", err.message().c_str());
            break;
        }

        m_send_socket.set_option(asio::ip::multicast::enable_loopback(false), err);
        if (err)
        {
            RUN_LOG_ERR("multicast helper init failure while send socket set option (disable-loopback) error (%s)", err.message().c_str());
            break;
        }

        m_send_run_thread = std::thread(&MulticastHelperImpl::context_run, this, true);
        if (!m_send_run_thread.joinable())
        {
            RUN_LOG_ERR("multicast helper init failure while send context run thread create failed");
            break;
        }

        if (nullptr != multicast_recver)
        {
            m_recv_socket.open(asio::ip::udp::v4(), err);
            if (err)
            {
                RUN_LOG_ERR("multicast helper init failure while recv socket open error (%s)", err.message().c_str());
                break;
            }

            m_recv_socket.set_option(asio::ip::multicast::enable_loopback(false), err);
            if (err)
            {
                RUN_LOG_ERR("multicast helper init failure while recv socket set option (disable-loopback) error (%s)", err.message().c_str());
                break;
            }

            m_recv_socket.set_option(asio::ip::udp::socket::reuse_address(true), err);
            if (err)
            {
                RUN_LOG_ERR("multicast helper init failure while recv socket set option (reuse-address) error (%s)", err.message().c_str());
                break;
            }

            m_recv_socket.bind(asio::ip::udp::endpoint(asio::ip::udp::v4(), multicast_group_port), err);
            if (err)
            {
                RUN_LOG_ERR("multicast helper init failure while recv socket bind (multicast) error (%s)", err.message().c_str());
                break;
            }

            m_recv_socket.set_option(asio::ip::multicast::join_group(asio::ip::address::from_string(multicast_group_host)), err);
            if (err)
            {
                RUN_LOG_ERR("multicast helper init failure while recv socket set option (multicast) error (%s)", err.message().c_str());
                break;
            }

            m_recv_handle_thread = std::thread(&MulticastHelperImpl::handle_data, this);
            if (!m_recv_handle_thread.joinable())
            {
                RUN_LOG_ERR("multicast helper init failure while recv data handle thread create failed");
                break;
            }

            m_recv_run_thread = std::thread(&MulticastHelperImpl::context_run, this, false);
            if (!m_recv_run_thread.joinable())
            {
                RUN_LOG_ERR("multicast helper init failure while recv context run thread create failed");
                break;
            }

            do_recv();
        }
        else
        {
            RUN_LOG_WAR("multicast helper init warning while multicast recver is invalid");
        }

        RUN_LOG_DBG("multicast helper init success");

        return true;
    } while (false);

    RUN_LOG_ERR("multicast helper init failure");

    exit();

    return false;
}

void MulticastHelperImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("multicast helper exit begin");

        m_running = false;

        if (m_send_run_thread.joinable())
        {
            RUN_LOG_DBG("multicast helper exit while send context stop");
            m_send_context.stop();
            RUN_LOG_DBG("multicast helper exit while send context run thread exit begin");
            m_send_run_thread.join();
            RUN_LOG_DBG("multicast helper exit while send context run thread exit end");
        }

        if (m_recv_run_thread.joinable())
        {
            RUN_LOG_DBG("multicast helper exit while recv context stop");
            m_recv_context.stop();
            RUN_LOG_DBG("multicast helper exit while recv context run thread exit begin");
            m_recv_run_thread.join();
            RUN_LOG_DBG("multicast helper exit while recv context run thread exit end");
        }

        if (m_recv_handle_thread.joinable())
        {
            RUN_LOG_DBG("multicast helper exit while recv data handle thread exit begin");
            m_recv_data_condition.notify_one();
            m_recv_handle_thread.join();
            RUN_LOG_DBG("multicast helper exit while recv data handle thread exit end");
        }

        m_send_data_list.clear();
        m_recv_data_list.clear();

        m_multicast_recver = nullptr;

        RUN_LOG_DBG("multicast helper exit end");
    }
}

void MulticastHelperImpl::do_recv()
{
    m_recv_socket.async_receive_from(asio::buffer(m_recv_buffer), m_peer_endpoint, [this](std::error_code err, std::size_t len){
        if (err)
        {
            if (!m_running || !m_recv_socket.is_open())
            {
                return;
            }

            RUN_LOG_ERR("multicast helper recv error: (%s)", err.message().c_str());
        }
        else
        {
            asio::error_code ec;
            std::string address(m_peer_endpoint.address().to_string(ec));
            std::string message(m_recv_buffer.data(), len);
            std::pair<std::string, std::string> data_pair(std::move(address), std::move(message));

            {
                std::lock_guard<std::mutex> locker(m_recv_data_mutex);

                m_recv_data_list.emplace_back(std::move(data_pair));
            }

            m_recv_data_condition.notify_one();
        }

        do_recv();
    });
}

void MulticastHelperImpl::do_send()
{
    if (m_send_data_list.empty())
    {
        RUN_LOG_CRI("multicast helper send data list is empty");
        return;
    }

    const std::string & data = m_send_data_list.front();
    m_send_socket.async_send_to(asio::buffer(data), m_multicast_endpoint, [this](const std::error_code & err, std::size_t len){
        if (err)
        {
            if (!m_running || !m_send_socket.is_open())
            {
                return;
            }

            RUN_LOG_ERR("multicast helper send error: (%s)", err.message().c_str());
        }
        else
        {
            if (m_send_data_list.empty())
            {
                RUN_LOG_CRI("multicast helper send data list is empty");
                return;
            }

            if (m_send_data_list.front().size() != len)
            {
                RUN_LOG_CRI("multicast helper send data size %u != %u", static_cast<uint32_t>(m_send_data_list.front().size()), static_cast<uint32_t>(len));
            }

            m_send_data_list.pop_front();
        }

        if (!m_send_data_list.empty())
        {
            do_send();
        }
    });
}

bool MulticastHelperImpl::send_data(const std::string & data)
{
    if (!m_running || data.empty())
    {
        return false;
    }

    std::string message;
    if (!packet_message(data, message))
    {
        return false;
    }

    asio::post(m_send_context, [this, packet = std::move(message)]{
        if (!m_running || !m_send_socket.is_open())
        {
            return;
        }

        m_send_data_list.emplace_back(std::move(packet));

        if (1 == m_send_data_list.size())
        {
            do_send();
        }
    });

    return true;
}

bool MulticastHelperImpl::packet_message(const std::string & data, std::string & message)
{
    if (data.empty() || m_protocol_prefix.size() + data.size() + m_protocol_suffix.size() > m_recv_buffer.size())
    {
        RUN_LOG_ERR("multicast helper packet message failure while data size is too big");
        return false;
    }

    message = m_protocol_prefix + data + m_protocol_suffix;

    return true;
}

bool MulticastHelperImpl::unpack_message(const std::string & message, std::string & data)
{
    if (message.size() <= m_protocol_prefix.size() + m_protocol_suffix.size())
    {
        return false;
    }

    if (!m_protocol_prefix.empty() && 0 != memcmp(message.data(), m_protocol_prefix.data(), m_protocol_prefix.size()))
    {
        return false;
    }

    if (!m_protocol_suffix.empty() && 0 != memcmp(message.data() + message.size() - m_protocol_suffix.size(), m_protocol_suffix.data(), m_protocol_suffix.size()))
    {
        return false;
    }

    data = message.substr(m_protocol_prefix.size(), message.size() - m_protocol_prefix.size() - m_protocol_suffix.size());

    return true;
}

void MulticastHelperImpl::handle_data()
{
    while (m_running)
    {
        std::list<std::pair<std::string, std::string>> recv_data_list;

        {
            std::unique_lock<std::mutex> locker(m_recv_data_mutex);

            while (m_running && m_recv_data_list.empty())
            {
                m_recv_data_condition.wait(locker);
            }

            recv_data_list.swap(m_recv_data_list);
        }

        if (recv_data_list.empty())
        {
            continue;
        }

        for (std::list<std::pair<std::string, std::string>>::iterator iter = recv_data_list.begin(); recv_data_list.end() != iter && m_running; ++iter)
        {
            const std::string & address = iter->first;
            const std::string & message = iter->second;
            std::string data;
            if (unpack_message(message, data))
            {
                m_multicast_recver->recv_multicast_data(m_multicast_group_host, m_multicast_group_port, address, data);
            }
        }
    }
}

void MulticastHelperImpl::context_run(bool send_not_recv)
{
    if (send_not_recv)
    {
        asio::io_context::work work(m_send_context);
        m_send_context.run();
    }
    else
    {
        asio::io_context::work work(m_recv_context);
        m_recv_context.run();
    }
}

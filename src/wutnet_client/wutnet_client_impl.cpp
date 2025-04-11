/********************************************************
 * Description : ws & udp & tcp network client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "wutnet_client_impl.h"
#include "base.h"

WutnetBase::~WutnetBase()
{

}

WutnetOnWebsocket::WutnetOnWebsocket(const void * user_data)
    : m_running(false)
    , m_client(user_data)
    , m_sink(nullptr)
    , m_name("websocket")
{

}

WutnetOnWebsocket::~WutnetOnWebsocket()
{
    exit();
}

bool WutnetOnWebsocket::init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data)
{
    m_running = true;

    m_sink = sink;

    if (m_client.init(this, target_host, target_port, target_secure, gateway_host, gateway_port, gateway_secure, connect_data))
    {
        return true;
    }

    exit();

    return false;
}

void WutnetOnWebsocket::exit()
{
    if (m_running)
    {
        m_running = false;
        m_client.exit();
        m_sink = nullptr;
    }
}

void WutnetOnWebsocket::connect()
{
    m_client.connect();
}

void WutnetOnWebsocket::close()
{
    m_client.close();
}

bool WutnetOnWebsocket::send_message(std::string && packet)
{
    return m_client.send_message(std::move(packet), true);
}

bool WutnetOnWebsocket::is_connected() const
{
    return m_client.is_connected();
}

void WutnetOnWebsocket::on_websocket_connect(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_connect(m_name, user_data);
    }
}

void WutnetOnWebsocket::on_websocket_close(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_close(m_name, user_data);
    }
}

void WutnetOnWebsocket::on_websocket_error(const void * user_data, const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_error(m_name, user_data, action, message);
    }
}

void WutnetOnWebsocket::on_websocket_recv(const void * user_data, std::string && packet, bool binary)
{
    if (nullptr != m_sink)
    {
        m_sink->on_recv(m_name, user_data, std::move(packet));
    }
}

WutnetOnEnet::WutnetOnEnet(const void * user_data)
    : m_running(false)
    , m_client(user_data)
    , m_sink(nullptr)
    , m_name("enet")
{

}

WutnetOnEnet::~WutnetOnEnet()
{
    exit();
}

bool WutnetOnEnet::init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data)
{
    m_running = true;

    m_sink = sink;

    if (nullptr == target_host || 0x0 == *target_host || 0 == target_port)
    {
        target_host = gateway_host;
        target_port = gateway_port;
    }

    if (m_client.init(this, target_host, target_port))
    {
        return true;
    }

    exit();

    return false;
}

void WutnetOnEnet::exit()
{
    if (m_running)
    {
        m_running = false;
        m_client.exit();
        m_sink = nullptr;
    }
}

void WutnetOnEnet::connect()
{
    m_client.connect();
}

void WutnetOnEnet::close()
{
    m_client.close();
}

bool WutnetOnEnet::send_message(std::string && packet)
{
    return m_client.send_message(std::move(packet));
}

bool WutnetOnEnet::is_connected() const
{
    return m_client.is_connected();
}

void WutnetOnEnet::on_enet_connect(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_connect(m_name, user_data);
    }
}

void WutnetOnEnet::on_enet_close(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_close(m_name, user_data);
    }
}

void WutnetOnEnet::on_enet_error(const void * user_data, const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_error(m_name, user_data, action, message);
    }
}

void WutnetOnEnet::on_enet_recv(const void * user_data, const void * data, uint32_t size)
{
    if (nullptr != m_sink)
    {
        std::string packet(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);
        m_sink->on_recv(m_name, user_data, std::move(packet));
    }
}

WutnetOnEtcp::WutnetOnEtcp(const void * user_data)
    : m_running(false)
    , m_client(user_data)
    , m_sink(nullptr)
    , m_name("etcp")
{

}

WutnetOnEtcp::~WutnetOnEtcp()
{
    exit();
}

bool WutnetOnEtcp::init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data)
{
    m_running = true;

    m_sink = sink;

    if (m_client.init(this, target_host, target_port, gateway_host, gateway_port, connect_data))
    {
        return true;
    }

    exit();

    return false;
}

void WutnetOnEtcp::exit()
{
    if (m_running)
    {
        m_running = false;
        m_client.exit();
        m_sink = nullptr;
    }
}

void WutnetOnEtcp::connect()
{
    m_client.connect();
}

void WutnetOnEtcp::close()
{
    m_client.close();
}

bool WutnetOnEtcp::send_message(std::string && packet)
{
    return m_client.send_message(std::move(packet));
}

bool WutnetOnEtcp::is_connected() const
{
    return m_client.is_connected();
}

void WutnetOnEtcp::on_etcp_connect(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_connect(m_name, user_data);
    }
}

void WutnetOnEtcp::on_etcp_close(const void * user_data)
{
    if (nullptr != m_sink)
    {
        m_sink->on_close(m_name, user_data);
    }
}

void WutnetOnEtcp::on_etcp_error(const void * user_data, const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_error(m_name, user_data, action, message);
    }
}

void WutnetOnEtcp::on_etcp_recv(const void * user_data, std::string && packet)
{
    if (nullptr != m_sink)
    {
        m_sink->on_recv(m_name, user_data, std::move(packet));
    }
}

WutnetClientImpl::WutnetClientImpl()
    : m_wutnet(nullptr)
{

}

WutnetClientImpl::~WutnetClientImpl()
{
    exit();
}

bool WutnetClientImpl::init(WutnetSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data, wutnet_type_t wutnet_type, const void * user_data)
{
    exit();

    do
    {
        switch (wutnet_type)
        {
            case wutnet_type_t::wutnet_on_ws:
            {
                m_wutnet = new WutnetOnWebsocket(user_data);
                break;
            }
            case wutnet_type_t::wutnet_on_enet:
            {
                m_wutnet = new WutnetOnEnet(user_data);
                break;
            }
            case wutnet_type_t::wutnet_on_etcp:
            {
                m_wutnet = new WutnetOnEtcp(user_data);
                break;
            }
            default:
            {
                RUN_LOG_ERR("wutnet client init failure while unknown wutnet type %u", static_cast<uint32_t>(wutnet_type));
                break;
            }
        }
        if (nullptr == m_wutnet)
        {
            break;
        }

        if (!m_wutnet->init(sink, target_host, target_port, false, gateway_host, gateway_port, false, connect_data))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void WutnetClientImpl::exit()
{
    if (nullptr != m_wutnet)
    {
        m_wutnet->exit();
        delete m_wutnet;
        m_wutnet = nullptr;
    }
}

void WutnetClientImpl::connect()
{
    if (nullptr != m_wutnet)
    {
        m_wutnet->connect();
    }
}

void WutnetClientImpl::close()
{
    if (nullptr != m_wutnet)
    {
        m_wutnet->close();
    }
}

bool WutnetClientImpl::send_message(std::string && packet)
{
    return nullptr != m_wutnet && m_wutnet->send_message(std::move(packet));
}

bool WutnetClientImpl::is_connected() const
{
    return nullptr != m_wutnet && m_wutnet->is_connected();
}

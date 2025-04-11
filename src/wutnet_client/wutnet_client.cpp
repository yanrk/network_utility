/********************************************************
 * Description : ws & udp & tcp network client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "wutnet_client.h"
#include "wutnet_client_impl.h"

WutnetSink::~WutnetSink()
{

}

WutnetClient::WutnetClient()
    : m_impl(nullptr)
{

}

WutnetClient::~WutnetClient()
{
    exit();
}

bool WutnetClient::init(WutnetSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data, wutnet_type_t wutnet_type, const void * user_data)
{
    exit();

    do
    {
        m_impl = new WutnetClientImpl;
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(sink, target_host, target_port, gateway_host, gateway_port, connect_data, wutnet_type, user_data))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void WutnetClient::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

void WutnetClient::connect()
{
    if (nullptr != m_impl)
    {
        m_impl->connect();
    }
}

void WutnetClient::close()
{
    if (nullptr != m_impl)
    {
        m_impl->close();
    }
}

bool WutnetClient::send_message(std::string && packet)
{
    return nullptr != m_impl && m_impl->send_message(std::move(packet));
}

bool WutnetClient::is_connected() const
{
    return nullptr != m_impl && m_impl->is_connected();
}

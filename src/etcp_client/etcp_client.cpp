/********************************************************
 * Description : easy tcp client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstring>
#include <string>
#include "etcp_client.h"
#include "etcp_client_impl.h"

EtcpClientSink::~EtcpClientSink()
{

}

EtcpClient::EtcpClient(const void * user_data)
    : m_impl(nullptr)
    , m_user_data(user_data)
{

}

EtcpClient::~EtcpClient()
{
    exit();
}

bool EtcpClient::init(EtcpClientSink * sink, const char * target_host, uint16_t target_port, const char * connect_data)
{
    return init(sink, target_host, target_port, nullptr, 0, connect_data);
}

bool EtcpClient::init(EtcpClientSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data)
{
    if (nullptr == target_host || 0x0 == *target_host || 0 == target_port)
    {
        target_host = "";
        target_port = 0;
    }

    if (nullptr == gateway_host || 0x0 == *gateway_host || 0 == gateway_port)
    {
        gateway_host = "";
        gateway_port = 0;
    }

    if (nullptr == connect_data)
    {
        connect_data = "";
    }

    exit();

    do
    {
        m_impl = new EtcpClientImpl(m_user_data);
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(sink, target_host, target_port, gateway_host, gateway_port, connect_data))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void EtcpClient::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

void EtcpClient::connect()
{
    if (nullptr != m_impl)
    {
        m_impl->connect();
    }
}

void EtcpClient::close()
{
    if (nullptr != m_impl)
    {
        m_impl->close();
    }
}

bool EtcpClient::send_message(const void * data, uint32_t size)
{
    std::string packet(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);
    return nullptr != m_impl && m_impl->send_message(std::move(packet));
}

bool EtcpClient::send_message(std::string && packet)
{
    return nullptr != m_impl && m_impl->send_message(std::move(packet));
}

bool EtcpClient::is_connected() const
{
    return nullptr != m_impl && m_impl->is_connected();
}

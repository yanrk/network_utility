/********************************************************
 * Description : enet client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#include "enet_client.h"
#include "enet_client_impl.h"

EnetClientSink::~EnetClientSink()
{

}

EnetClient::EnetClient(const void * user_data)
    : m_impl(nullptr)
    , m_user_data(user_data)
{

}

EnetClient::~EnetClient()
{
    exit();
}

bool EnetClient::init(EnetClientSink * sink, const char * host, uint16_t port)
{
    exit();

    do
    {
        m_impl = new EnetClientImpl(m_user_data);
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(sink, host, port))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void EnetClient::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

void EnetClient::connect()
{
    if (nullptr != m_impl)
    {
        m_impl->connect();
    }
}

void EnetClient::close()
{
    if (nullptr != m_impl)
    {
        m_impl->close();
    }
}

bool EnetClient::send_message(const void * data, uint32_t size)
{
    std::string packet(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);
    return nullptr != m_impl && m_impl->send_message(std::move(packet));
}

bool EnetClient::send_message(std::string && packet)
{
    return nullptr != m_impl && m_impl->send_message(std::move(packet));
}

bool EnetClient::is_connected() const
{
    return nullptr != m_impl && m_impl->is_connected();
}

/********************************************************
 * Description : easy tcp server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstring>
#include <string>
#include "etcp_server.h"
#include "etcp_server_impl.h"

EtcpConnection::EtcpConnection()
    : m_user_data(nullptr)
{

}

EtcpConnection::~EtcpConnection()
{

}

void EtcpConnection::set_user_data(void * user_data)
{
    m_user_data = user_data;
}

void * EtcpConnection::get_user_data()
{
    return m_user_data;
}

EtcpServerSink::~EtcpServerSink()
{

}

EtcpServer::EtcpServer()
    : m_impl(nullptr)
{

}

EtcpServer::~EtcpServer()
{
    exit();
}

bool EtcpServer::init(EtcpServerSink * sink, const char * host, uint16_t port, uint32_t thread_count)
{
    if (nullptr == host || 0x0 == *host)
    {
        host = "0.0.0.0";
    }

    exit();

    do
    {
        m_impl = new EtcpServerImpl;
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(sink, host, port, thread_count))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void EtcpServer::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

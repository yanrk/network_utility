/********************************************************
 * Description : tcp gateway server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include "tcp_gateway_server.h"
#include "tcp_gateway_server_impl.h"

TcpGatewayServer::TcpGatewayServer()
    : m_impl(nullptr)
{

}

TcpGatewayServer::~TcpGatewayServer()
{
    exit();
}

bool TcpGatewayServer::init(const char * host, uint16_t port, uint32_t thread_count)
{
    if (nullptr == host || 0x0 == *host)
    {
        host = "0.0.0.0";
    }

    if (0 == thread_count)
    {
        thread_count = 1;
    }

    exit();

    do
    {
        m_impl = new TcpGatewayServerImpl;
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(host, port, thread_count))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void TcpGatewayServer::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

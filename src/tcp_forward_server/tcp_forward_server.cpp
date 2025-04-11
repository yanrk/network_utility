/********************************************************
 * Description : tcp forward server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include "tcp_forward_server.h"
#include "tcp_forward_server_impl.h"

TcpForwardServer::TcpForwardServer()
    : m_impl(nullptr)
{

}

TcpForwardServer::~TcpForwardServer()
{
    exit();
}

bool TcpForwardServer::init(const char * gateway_host, uint16_t gateway_port)
{
    if (nullptr == gateway_host)
    {
        gateway_host = "";
    }

    exit();

    do
    {
        m_impl = new TcpForwardServerImpl;
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(gateway_host, gateway_port))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void TcpForwardServer::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

uint16_t TcpForwardServer::acquire_local_forward_port(const char * target_host, uint16_t target_port)
{
    if (nullptr == target_host)
    {
        target_host = "";
    }
    return nullptr != m_impl ? m_impl->acquire_local_forward_port(target_host, target_port) : 0;
}

void TcpForwardServer::release_local_forward_port(uint16_t listen_port)
{
    if (nullptr != m_impl && 0 != listen_port)
    {
        m_impl->release_local_forward_port(listen_port);
    }
}

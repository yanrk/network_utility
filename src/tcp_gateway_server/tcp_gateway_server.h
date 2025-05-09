/********************************************************
 * Description : tcp gateway server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_GATEWAY_SERVER_H
#define TCP_GATEWAY_SERVER_H


#include <cstdint>
#include "macros.h"

class TcpGatewayServerImpl;

class GOOFER_API TcpGatewayServer
{
public:
    TcpGatewayServer();
    ~TcpGatewayServer();

public:
    bool init(const char * host, uint16_t port, uint32_t thread_count);
    void exit();

private:
    TcpGatewayServer(const TcpGatewayServer &) = delete;
    TcpGatewayServer(TcpGatewayServer &&) = delete;
    TcpGatewayServer & operator = (const TcpGatewayServer &) = delete;
    TcpGatewayServer & operator = (TcpGatewayServer &&) = delete;

private:
    TcpGatewayServerImpl          * m_impl;
};


#endif // TCP_GATEWAY_SERVER_H

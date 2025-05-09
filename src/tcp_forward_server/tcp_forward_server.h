/********************************************************
 * Description : tcp forward server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_FORWARD_SERVER_H
#define TCP_FORWARD_SERVER_H


#include <cstdint>
#include "macros.h"

class TcpForwardServerImpl;

class GOOFER_API TcpForwardServer
{
public:
    TcpForwardServer();
    ~TcpForwardServer();

public:
    bool init(const char * gateway_host, uint16_t gateway_port);
    void exit();

public:
    uint16_t acquire_local_forward_port(const char * target_host, uint16_t target_port);
    void release_local_forward_port(uint16_t listen_port);

private:
    TcpForwardServer(const TcpForwardServer &) = delete;
    TcpForwardServer(TcpForwardServer &&) = delete;
    TcpForwardServer & operator = (const TcpForwardServer &) = delete;
    TcpForwardServer & operator = (TcpForwardServer &&) = delete;

private:
    TcpForwardServerImpl          * m_impl;
};


#endif // TCP_FORWARD_SERVER_H

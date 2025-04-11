/********************************************************
 * Description : tcp forward server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_FORWARD_SERVER_IMPL_H
#define TCP_FORWARD_SERVER_IMPL_H


#include <map>
#include <vector>
#include <string>
#include <thread>
#include <memory>
#include <random>
#include <mutex>
#include "asio.hpp"
#include "tcp_forward_server.h"
#include "tcp_forward_acceptor.h"

class TcpForwardServerImpl
{
public:
    TcpForwardServerImpl();
    ~TcpForwardServerImpl();

public:
    bool init(const std::string & gateway_host, uint16_t gateway_port);
    void exit();

public:
    uint16_t acquire_local_forward_port(const std::string & target_host, uint16_t target_port);
    void release_local_forward_port(uint16_t listen_port);

private:
    void release_all_local_forward_ports();
    std::vector<uint8_t> make_forward_message(uint32_t addr, uint16_t port);

private:
    TcpForwardServerImpl(const TcpForwardServerImpl &) = delete;
    TcpForwardServerImpl(TcpForwardServerImpl &&) = delete;
    TcpForwardServerImpl & operator = (const TcpForwardServerImpl &) = delete;
    TcpForwardServerImpl & operator = (TcpForwardServerImpl &&) = delete;

private:
    bool                                                        m_running;
    std::knuth_b                                                m_random;
    std::string                                                 m_gateway_host;
    uint16_t                                                    m_gateway_port;
    asio::ip::tcp::endpoint                                     m_gateway_endpoint;
    asio::io_context                                            m_io_context;
    std::unique_ptr<asio::io_context::work>                     m_work;
    std::thread                                                 m_work_thread;
    std::map<uint16_t, std::unique_ptr<TcpForwardAcceptor>>     m_acceptor_map;
    std::mutex                                                  m_acceptor_mutex;
};


#endif // TCP_FORWARD_SERVER_IMPL_H

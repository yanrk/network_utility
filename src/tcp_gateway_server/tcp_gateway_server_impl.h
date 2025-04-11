/********************************************************
 * Description : tcp gateway server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_GATEWAY_SERVER_IMPL_H
#define TCP_GATEWAY_SERVER_IMPL_H


#include <list>
#include <string>
#include <thread>
#include <memory>
#include "asio.hpp"
#include "tcp_gateway_server.h"

class TcpGatewayServerImpl
{
public:
    TcpGatewayServerImpl();
    ~TcpGatewayServerImpl();

public:
    bool init(const std::string & host, uint16_t port, uint32_t thread_count);
    void exit();

private:
    void do_accept();
    void do_recv_head(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket);
    void do_recv_body(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket);
    void do_send_result(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket, std::shared_ptr<char> response, uint32_t response_size, std::shared_ptr<bool> result, std::shared_ptr<asio::ip::tcp::endpoint> endpoint);

private:
    void get_address(const asio::ip::tcp::endpoint & endpoint, std::string & host, uint16_t & port);

private:
    asio::io_context & get_io_context();

private:
    void run(asio::io_context * io_context);

private:
    TcpGatewayServerImpl(const TcpGatewayServerImpl &) = delete;
    TcpGatewayServerImpl(TcpGatewayServerImpl &&) = delete;
    TcpGatewayServerImpl & operator = (const TcpGatewayServerImpl &) = delete;
    TcpGatewayServerImpl & operator = (TcpGatewayServerImpl &&) = delete;

private:
    bool                                                    m_running;
    std::unique_ptr<asio::ip::tcp::acceptor>                m_acceptor;
    std::list<std::unique_ptr<asio::io_context::work>>      m_io_works;
    std::list<std::unique_ptr<asio::io_context>>            m_io_contexts;
    std::list<std::unique_ptr<asio::io_context>>::iterator  m_io_context_iter;
    std::list<std::unique_ptr<std::thread>>                 m_work_threads;
};


#endif // TCP_GATEWAY_SERVER_IMPL_H

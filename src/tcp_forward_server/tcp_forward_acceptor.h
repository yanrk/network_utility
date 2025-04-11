/********************************************************
 * Description : tcp forward acceptor
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_FORWARD_ACCEPTOR_H
#define TCP_FORWARD_ACCEPTOR_H


#include <cstdint>
#include <string>
#include <vector>
#include "asio.hpp"

class TcpForwardAcceptor
{
public:
    TcpForwardAcceptor(asio::io_context & io_context, const asio::ip::tcp::endpoint & forward_endpoint, const asio::ip::tcp::endpoint & target_endpoint, std::vector<uint8_t> && message);
    ~TcpForwardAcceptor();

public:
    bool start();
    void stop();

public:
    uint16_t get_listen_port() const;

private:
    bool listen();
    bool do_accept();
    void get_address(const asio::ip::tcp::endpoint & endpoint, std::string & host, uint16_t & port);

private:
    TcpForwardAcceptor(const TcpForwardAcceptor &) = delete;
    TcpForwardAcceptor(TcpForwardAcceptor &&) = delete;
    TcpForwardAcceptor & operator = (const TcpForwardAcceptor &) = delete;
    TcpForwardAcceptor & operator = (TcpForwardAcceptor &&) = delete;

private:
    volatile bool                                   m_running;
    asio::io_context                              & m_io_context;
    asio::ip::tcp::acceptor                         m_acceptor;
    const asio::ip::tcp::endpoint                   m_gateway_endpoint;
    std::string                                     m_gateway_host;
    uint16_t                                        m_gateway_port;
    const asio::ip::tcp::endpoint                   m_target_endpoint;
    std::string                                     m_target_host;
    uint16_t                                        m_target_port;
    uint16_t                                        m_listen_port;
    std::vector<uint8_t>                            m_message;
};


#endif // TCP_FORWARD_ACCEPTOR_H

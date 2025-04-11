/********************************************************
 * Description : tcp gateway session
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_GATEWAY_SESSION_H
#define TCP_GATEWAY_SESSION_H


#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "asio.hpp"

class TcpGatewaySession : public std::enable_shared_from_this<TcpGatewaySession>
{
public:
    TcpGatewaySession(asio::ip::tcp::socket && local_socket, asio::ip::tcp::socket && remote_socket, const std::string & local_host, uint16_t local_port, const std::string & remote_host, uint16_t remote_port);
    ~TcpGatewaySession();

public:
    void start();
    void stop();

private:
    void close(asio::ip::tcp::socket & socket);
    void set_options(asio::ip::tcp::socket & socket);
    void do_recv(asio::ip::tcp::socket & recv_socket, asio::ip::tcp::socket & send_socket, std::vector<uint8_t> & recv_buffer, bool local_recv);

private:
    TcpGatewaySession(const TcpGatewaySession &) = delete;
    TcpGatewaySession(TcpGatewaySession &&) = delete;
    TcpGatewaySession & operator = (const TcpGatewaySession &) = delete;
    TcpGatewaySession & operator = (TcpGatewaySession &&) = delete;

private:
    asio::ip::tcp::socket                           m_local_socket;
    std::string                                     m_local_host;
    uint16_t                                        m_local_port;
    std::vector<uint8_t>                            m_local_buffer;

private:
    asio::ip::tcp::socket                           m_remote_socket;
    std::string                                     m_remote_host;
    uint16_t                                        m_remote_port;
    std::vector<uint8_t>                            m_remote_buffer;
};


#endif // TCP_GATEWAY_SESSION_H

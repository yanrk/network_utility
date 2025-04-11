/********************************************************
 * Description : tcp forward session
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef TCP_FORWARD_SESSION_H
#define TCP_FORWARD_SESSION_H


#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include "asio.hpp"

class TcpForwardSession : public std::enable_shared_from_this<TcpForwardSession>
{
public:
    TcpForwardSession(asio::ip::tcp::socket && local_socket, asio::ip::tcp::socket && remote_socket, const std::string & local_host, uint16_t local_port, const std::string & remote_host, uint16_t remote_port);
    ~TcpForwardSession();

public:
    void start();
    void stop();

private:
    void close(asio::ip::tcp::socket & socket);
    void set_options(asio::ip::tcp::socket & socket);
    void do_recv(asio::ip::tcp::socket & recv_socket, asio::ip::tcp::socket & send_socket, std::vector<uint8_t> & recv_buffer, bool local_recv);

private:
    TcpForwardSession(const TcpForwardSession &) = delete;
    TcpForwardSession(TcpForwardSession &&) = delete;
    TcpForwardSession & operator = (const TcpForwardSession &) = delete;
    TcpForwardSession & operator = (TcpForwardSession &&) = delete;

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


#endif // TCP_FORWARD_SESSION_H

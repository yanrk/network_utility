/********************************************************
 * Description : tcp gateway session
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include "tcp_gateway_session.h"
#include "base.h"

TcpGatewaySession::TcpGatewaySession(asio::ip::tcp::socket && local_socket, asio::ip::tcp::socket && remote_socket, const std::string & local_host, uint16_t local_port, const std::string & remote_host, uint16_t remote_port)
    : m_local_socket(std::move(local_socket))
    , m_local_host(local_host)
    , m_local_port(local_port)
    , m_local_buffer(32 * 1024)
    , m_remote_socket(std::move(remote_socket))
    , m_remote_host(remote_host)
    , m_remote_port(remote_port)
    , m_remote_buffer(32 * 1024)
{

}

TcpGatewaySession::~TcpGatewaySession()
{

}

void TcpGatewaySession::start()
{
    set_options(m_local_socket);
    set_options(m_remote_socket);
    do_recv(m_local_socket, m_remote_socket, m_local_buffer, true);
    do_recv(m_remote_socket, m_local_socket, m_remote_buffer, false);
}

void TcpGatewaySession::stop()
{
    close(m_local_socket);
    close(m_remote_socket);
}

void TcpGatewaySession::close(asio::ip::tcp::socket & socket)
{
    std::error_code err;
    socket.shutdown(asio::socket_base::shutdown_both, err);
    socket.close(err);
}

void TcpGatewaySession::set_options(asio::ip::tcp::socket & socket)
{
    std::error_code err;

    socket.set_option(asio::ip::tcp::no_delay(true), err);
    if (err)
    {
        RUN_LOG_ERR("tcp gateway session (%s:%u, %s:%u) set option (no-delay) error (%s)", m_local_host.c_str(), m_local_port, m_remote_host.c_str(), m_remote_port, err.message().c_str());
    }

    socket.set_option(asio::socket_base::send_buffer_size(4 * 1024 * 1024), err);
    if (err)
    {
        RUN_LOG_ERR("tcp gateway session (%s:%u, %s:%u) set option (send-buffer-size) error (%s)", m_local_host.c_str(), m_local_port, m_remote_host.c_str(), m_remote_port, err.message().c_str());
    }

    socket.set_option(asio::socket_base::receive_buffer_size(4 * 1024 * 1024), err);
    if (err)
    {
        RUN_LOG_ERR("tcp gateway session (%s:%u, %s:%u) set option (recv-buffer-size) error (%s)", m_local_host.c_str(), m_local_port, m_remote_host.c_str(), m_remote_port, err.message().c_str());
    }
}

void TcpGatewaySession::do_recv(asio::ip::tcp::socket & recv_socket, asio::ip::tcp::socket & send_socket, std::vector<uint8_t> & recv_buffer, bool local_recv)
{
    std::shared_ptr<TcpGatewaySession> self(shared_from_this());

    recv_socket.async_read_some(asio::buffer(recv_buffer), [&, this, self, local_recv](std::error_code err, std::size_t len){
        if (err)
        {
            RUN_LOG_ERR("tcp gateway session (%s:%u, %s:%u) %s recv error (%s)", m_local_host.c_str(), m_local_port, m_remote_host.c_str(), m_remote_port, (local_recv ? "local" : "remote"), err.message().c_str());
            stop();
            return;
        }

        asio::async_write(send_socket, asio::buffer(recv_buffer.data(), len), asio::transfer_all(), [&, this, self, local_recv](std::error_code err, std::size_t len){
            if (err)
            {
                RUN_LOG_ERR("tcp gateway session (%s:%u, %s:%u) %s send error (%s)", m_local_host.c_str(), m_local_port, m_remote_host.c_str(), m_remote_port, (local_recv ? "remote" : "local"), err.message().c_str());
                stop();
                return;
            }

            do_recv(recv_socket, send_socket, recv_buffer, local_recv);
        });
    });
}

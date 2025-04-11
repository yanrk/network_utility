/********************************************************
 * Description : tcp forward acceptor
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include "tcp_forward_acceptor.h"
#include "tcp_forward_session.h"
#include "base.h"

TcpForwardAcceptor::TcpForwardAcceptor(asio::io_context & io_context, const asio::ip::tcp::endpoint & forward_endpoint, const asio::ip::tcp::endpoint & target_endpoint, std::vector<uint8_t> && message)
    : m_running(false)
    , m_io_context(io_context)
    , m_acceptor(m_io_context)
    , m_gateway_endpoint(forward_endpoint)
    , m_gateway_host()
    , m_gateway_port(0)
    , m_target_endpoint(target_endpoint)
    , m_target_host()
    , m_target_port(0)
    , m_listen_port(0)
    , m_message(std::move(message))
{
    get_address(m_gateway_endpoint, m_gateway_host, m_gateway_port);
    get_address(m_target_endpoint, m_target_host, m_target_port);
}

TcpForwardAcceptor::~TcpForwardAcceptor()
{
    stop();
}

bool TcpForwardAcceptor::start()
{
    stop();
    m_running = true;
    return listen() && do_accept();
}

void TcpForwardAcceptor::stop()
{
    if (m_running)
    {
        m_running = false;
        std::error_code err;
        m_acceptor.cancel(err);
        m_acceptor.close(err);
        m_listen_port = 0;
    }
}

uint16_t TcpForwardAcceptor::get_listen_port() const
{
    return m_listen_port;
}

void TcpForwardAcceptor::get_address(const asio::ip::tcp::endpoint & endpoint, std::string & host, uint16_t & port)
{
    std::error_code err;
    host = endpoint.address().to_string(err);
    port = endpoint.port();
}

bool TcpForwardAcceptor::listen()
{
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 0);

    std::error_code err;
    m_acceptor.open(endpoint.protocol(), err);
    if (err)
    {
        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) open error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
        return false;
    }

    m_acceptor.bind(endpoint, err);
    if (err)
    {
        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) bind error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
        return false;
    }

    m_acceptor.listen();

    m_listen_port = m_acceptor.local_endpoint(err).port();
    if (0 == m_listen_port)
    {
        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) get port error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
        return false;
    }

    return true;
}

bool TcpForwardAcceptor::do_accept()
{
    std::shared_ptr<asio::ip::tcp::socket> host_socket = std::make_shared<asio::ip::tcp::socket>(m_io_context);
    if (!host_socket)
    {
        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) create host socket failed", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port);
        return false;
    }

    std::shared_ptr<asio::ip::tcp::socket> peer_socket = std::make_shared<asio::ip::tcp::socket>(m_io_context);
    if (!peer_socket)
    {
        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) create peer socket failed", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port);
        return false;
    }

    m_acceptor.async_accept(*host_socket, [=](const std::error_code & err){
        if (err)
        {
            if (m_running)
            {
                RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) async accept error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
            }
            return;
        }

        peer_socket->async_connect(m_gateway_endpoint, [=](std::error_code err){
            if (err)
            {
                RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) async connect error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
                return;
            }

            asio::async_write(*peer_socket, asio::buffer(m_message), asio::transfer_all(), [=](std::error_code err, std::size_t len){
                if (err)
                {
                    RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) async write error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
                    return;
                }

                std::shared_ptr<uint8_t> result = std::make_shared<uint8_t>(0);
                asio::async_read(*peer_socket, asio::buffer(result.get(), 1), asio::transfer_exactly(1), [=](std::error_code err, std::size_t len){
                    if (err)
                    {
                        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) async read error (%s)", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port, err.message().c_str());
                        return;
                    }

                    if (0 == *result)
                    {
                        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) forward check failed", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port);
                        return;
                    }

                    std::string source_host;
                    uint16_t source_port = 0;
                    get_address(host_socket->remote_endpoint(err), source_host, source_port);

                    std::shared_ptr<TcpForwardSession> session = std::make_shared<TcpForwardSession>(std::move(*host_socket), std::move(*peer_socket), source_host, source_port, m_target_host, m_target_port);
                    if (!session)
                    {
                        RUN_LOG_ERR("tcp forward acceptor (%s:%u, %s:%u) create tcp forward session failed", m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port);
                        return;
                    }

                    RUN_LOG_DBG("tcp forward: (%s:%u) <--> (%s:%u) <-||-> (%s:%u) <--> (%s:%u)", source_host.c_str(), source_port, "127.0.0.1", m_listen_port, m_gateway_host.c_str(), m_gateway_port, m_target_host.c_str(), m_target_port);

                    session->start();
                });
            });
        });

        do_accept();
    });

    return true;
}

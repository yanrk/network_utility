/********************************************************
 * Description : easy tcp session
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "etcp_session.h"
#include "etcp_server_impl.h"

#define PROTOCOL_SIGN 0x4554 // 'ET'

EtcpSession::EtcpSession(EtcpServerSink * sink, EtcpServerImpl & server, asio::io_context & io_context)
    : m_closed(true)
    , m_sink(sink)
    , m_server(server)
    , m_io_context(io_context)
    , m_socket(io_context)
    , m_host_ip()
    , m_host_port(0)
    , m_peer_ip()
    , m_peer_port(0)
    , m_request()
    , m_response()
    , m_recv_head()
    , m_recv_data()
    , m_send_data_list()
{

}

EtcpSession::~EtcpSession()
{
    close();
}

asio::ip::tcp::socket & EtcpSession::get_socket()
{
    return m_socket;
}

void EtcpSession::start()
{
    std::error_code err;
    m_host_ip = m_socket.local_endpoint(err).address().to_string(err);
    m_host_port = m_socket.local_endpoint(err).port();
    m_peer_ip = m_socket.remote_endpoint(err).address().to_string(err);
    m_peer_port = m_socket.remote_endpoint(err).port();

    on_accept();
    do_read();
}

void EtcpSession::get_host_address(std::string & ip, unsigned short & port)
{
    ip = m_host_ip;
    port = m_host_port;
}

void EtcpSession::get_peer_address(std::string & ip, unsigned short & port)
{
    ip = m_peer_ip;
    port = m_peer_port;
}

void EtcpSession::close()
{
    if (!m_closed)
    {
        do_close();
    }
}

bool EtcpSession::send_message(const void * data, uint32_t size)
{
    if (!m_socket.is_open() || nullptr == data || 0 == size)
    {
        return false;
    }

    std::string packet(reinterpret_cast<const char *>(data), reinterpret_cast<const char *>(data) + size);
    return send_message(std::move(packet));
}

bool EtcpSession::send_message(std::string && packet)
{
    if (!m_socket.is_open() || packet.empty())
    {
        return false;
    }

    asio::post(m_io_context, [this, self = shared_from_this(), pack = std::move(packet)]{
        if (!m_socket.is_open())
        {
            return;
        }

        std::string head(sizeof(EtcpPrefix), 0x0);

        EtcpPrefix * prefix = reinterpret_cast<EtcpPrefix *>(&head[0]);
        prefix->sign = PROTOCOL_SIGN;
        prefix->size = static_cast<uint32_t>(pack.size());
        prefix->encode();

        m_send_data_list.emplace_back(std::move(head));
        m_send_data_list.emplace_back(std::move(pack));

        if (2 == m_send_data_list.size())
        {
            do_write();
        }
    });

    return true;
}

void EtcpSession::on_accept()
{
    if (m_closed)
    {
        m_closed = false;
        if (nullptr != m_sink)
        {
            m_sink->on_etcp_accept(shared_from_this());
        }
        m_server.append_session(shared_from_this());
    }
}

void EtcpSession::on_close()
{
    if (!m_closed)
    {
        m_closed = true;
        if (nullptr != m_sink)
        {
            m_sink->on_etcp_close(shared_from_this());
        }
        m_server.remove_session(shared_from_this());
    }
}

void EtcpSession::on_error(const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_etcp_error(shared_from_this(), action, message);
    }
}

void EtcpSession::do_close()
{
    asio::post(m_io_context, [this, self = shared_from_this()]{
        if (m_socket.is_open())
        {
            std::error_code err;
            m_socket.shutdown(asio::socket_base::shutdown_both, err);
            m_socket.close(err);
        }
        on_close();
    });
}

void EtcpSession::do_write()
{
    if (m_send_data_list.empty())
    {
        RUN_LOG_CRI("send data list is empty");
        return;
    }

    const std::string & data = m_send_data_list.front();
    asio::async_write(m_socket, asio::buffer(data.data(), data.size()), asio::transfer_all(), [this, self = shared_from_this()](const std::error_code & err, std::size_t len){
        if (err)
        {
            m_send_data_list.clear();
            return;
        }

        if (!m_send_data_list.empty())
        {
            if (m_send_data_list.front().size() != len)
            {
                RUN_LOG_CRI("send data size %u != %u", static_cast<uint32_t>(m_send_data_list.front().size()), static_cast<uint32_t>(len));
            }
            m_send_data_list.pop_front();
        }
        else
        {
            RUN_LOG_CRI("send data list is empty");
        }

        if (!m_send_data_list.empty())
        {
            do_write();
        }
    });
}

void EtcpSession::do_read()
{
    asio::async_read(m_socket, asio::buffer(m_recv_head, sizeof(m_recv_head)), asio::transfer_all(), [this, self = shared_from_this()](const std::error_code & err, std::size_t len){
        if (err)
        {
            on_close();
            return;
        }

        EtcpPrefix * prefix = reinterpret_cast<EtcpPrefix *>(m_recv_head);
        prefix->decode();

        if (PROTOCOL_SIGN != prefix->sign)
        {
            on_error("recv", "protocol sign check failed");
            do_close();
            return;
        }

        if (0 == prefix->size)
        {
            do_read();
            return;
        }

        m_recv_data.resize(prefix->size, 0x0);

        asio::async_read(m_socket, asio::buffer(&m_recv_data[0], m_recv_data.size()), asio::transfer_all(), [this, self = shared_from_this()](const std::error_code & err, std::size_t len){
            if (err)
            {
                on_close();
                return;
            }

            do_read();

            if (m_recv_data.size() != len)
            {
                RUN_LOG_CRI("recv data size %u != %u", static_cast<uint32_t>(m_recv_data.size()), static_cast<uint32_t>(len));
                m_recv_data.resize(len, 0x0);
            }

            if (nullptr != m_sink)
            {
                m_sink->on_etcp_recv(shared_from_this(), std::move(m_recv_data));
            }
        });
    });
}

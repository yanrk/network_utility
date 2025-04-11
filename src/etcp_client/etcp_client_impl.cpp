/********************************************************
 * Description : easy tcp client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#include "etcp_client_impl.h"

#define PROTOCOL_SIGN 0x4554 // 'ET'

EtcpClientImpl::EtcpClientImpl(const void * user_data)
    : m_running(false)
    , m_closed(true)
    , m_user_data(user_data)
    , m_sink(nullptr)
    , m_request()
    , m_response()
    , m_endpoint()
    , m_io_context()
    , m_work()
    , m_socket(m_io_context)
    , m_recv_head()
    , m_recv_data()
    , m_send_data_list()
    , m_work_thread()
{

}

EtcpClientImpl::~EtcpClientImpl()
{
    exit();
}

bool EtcpClientImpl::init(EtcpClientSink * sink, const std::string & target_host, uint16_t target_port, const std::string & gateway_host, uint16_t gateway_port, const std::string & connect_data)
{
    exit();

    RUN_LOG_DBG("etcp client init begin");

    m_running = true;
    m_closed = true;
    m_sink = sink;

    do
    {
        if (!target_host.empty() && 0 != target_port)
        {
            if (!gateway_host.empty() && 0 != gateway_port)
            {
                m_request = "CONNECT " + target_host + ":" + std::to_string(target_port) + " HTTP/1.0\r\n";
                if (!connect_data.empty())
                {
                    m_request += "user_data:" + connect_data + "\r\n";
                }
                m_request += "\r\n";
                m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(gateway_host), gateway_port);
            }
            else
            {
                m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(target_host), target_port);
            }
        }
        else
        {
            if (!gateway_host.empty() && 0 != gateway_port)
            {
                m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(gateway_host), gateway_port);
            }
            else
            {
                RUN_LOG_ERR("etcp client init failure while invalid paramenters");
                break;
            }
        }

        m_work = std::make_unique<asio::io_service::work>(m_io_context);
        if (!m_work)
        {
            RUN_LOG_ERR("etcp client init failure while create io context work failed");
            break;
        }

        m_work_thread = std::thread([this]{ m_io_context.run(); });
        if (!m_work_thread.joinable())
        {
            RUN_LOG_ERR("etcp client init failure while create work thread failed");
            break;
        }

        RUN_LOG_DBG("etcp client init success");

        return true;
    } while (false);

    RUN_LOG_ERR("etcp client init failure");

    exit();

    return false;
}

void EtcpClientImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("etcp client exit begin");

        m_running = false;

        m_work.reset();
        m_io_context.stop();

        if (m_work_thread.joinable())
        {
            m_work_thread.join();
        }

        m_request.clear();
        m_send_data_list.clear();

        RUN_LOG_DBG("etcp client exit end");
    }
}

void EtcpClientImpl::connect()
{
    if (m_running)
    {
        if (is_connected())
        {
            do_close();
        }
        do_connect();
    }
}

void EtcpClientImpl::close()
{
    if (m_running)
    {
        do_close();
    }
}

void EtcpClientImpl::on_connect()
{
    if (nullptr != m_sink)
    {
        m_sink->on_etcp_connect(m_user_data);
    }
}

void EtcpClientImpl::on_close()
{
    if (nullptr != m_sink && !m_closed)
    {
        m_sink->on_etcp_close(m_user_data);
    }
    m_closed = true;
}

void EtcpClientImpl::on_error(const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_etcp_error(m_user_data, action, message);
    }
}

bool EtcpClientImpl::send_message(std::string && packet)
{
    if (!m_running || !is_connected() || packet.empty())
    {
        return false;
    }

    asio::post(m_io_context, [this, pack = std::move(packet)]{
        if (!m_socket.is_open())
        {
            return;
        }

        std::string head(sizeof(EtcpHeader), 0x0);

        EtcpHeader * header = reinterpret_cast<EtcpHeader *>(&head[0]);
        header->sign = PROTOCOL_SIGN;
        header->size = static_cast<uint32_t>(pack.size());
        header->encode();

        m_send_data_list.emplace_back(std::move(head));
        m_send_data_list.emplace_back(std::move(pack));

        if (2 == m_send_data_list.size())
        {
            do_write();
        }
    });

    return true;
}

bool EtcpClientImpl::is_connected() const
{
    return m_socket.is_open();
}

void EtcpClientImpl::do_connect()
{
    asio::post(m_io_context, [this]{
        real_connect();
    });
}

void EtcpClientImpl::do_close()
{
    asio::post(m_io_context, [this]{
        if (is_connected())
        {
            real_close();
            on_close();
        }
    });
}

void EtcpClientImpl::do_proxy()
{
    asio::async_write(m_socket, asio::buffer(m_request.data(), m_request.size()) , asio::transfer_all(), [this](const std::error_code & err, std::size_t len){
        if (err)
        {
            const std::string message("http proxy send request error: " + err.message());
            on_error("connect", message.c_str());
            real_close();
            on_close();
            return;
        }

        m_response.prepare(1024);

        asio::async_read_until(m_socket, m_response, "\r\n\r\n", [this](const std::error_code & err, std::size_t len){
            if (err)
            {
                const std::string message("http proxy recv request error: " + err.message());
                on_error("connect", message.c_str());
                real_close();
                on_close();
                return;
            }

            float http_version = 0.f;
            int http_status = 0;
            std::sscanf(reinterpret_cast<const char *>(m_response.data().data()), "HTTP/%f %d", &http_version, &http_status);

            m_response.consume(len);

            if (2 != http_status / 100)
            {
                const std::string message("http proxy recv response status: " + std::to_string(http_status));
                on_error("connect", message.c_str());
                real_close();
                on_close();
                return;
            }

            do_read();

            on_connect();
        });
    });
}

void EtcpClientImpl::do_write()
{
    if (m_send_data_list.empty())
    {
        RUN_LOG_CRI("send data list is empty");
        return;
    }

    const std::string & data = m_send_data_list.front();
    asio::async_write(m_socket, asio::buffer(data.data(), data.size()), asio::transfer_all(), [this](const std::error_code & err, std::size_t len){
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

void EtcpClientImpl::do_read()
{
    asio::async_read(m_socket, asio::buffer(m_recv_head, sizeof(m_recv_head)), asio::transfer_all(), [this](const std::error_code & err, std::size_t len){
        if (err)
        {
            on_close();
            return;
        }

        EtcpHeader * header = reinterpret_cast<EtcpHeader *>(m_recv_head);
        header->decode();

        if (PROTOCOL_SIGN != header->sign)
        {
            on_error("recv", "protocol sign check failed");
            real_close();
            on_close();
            return;
        }

        if (0 == header->size)
        {
            do_read();
            return;
        }

        m_recv_data.resize(header->size, 0x0);

        asio::async_read(m_socket, asio::buffer(&m_recv_data[0], m_recv_data.size()), asio::transfer_all(), [this](const std::error_code & err, std::size_t len){
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
                m_sink->on_etcp_recv(m_user_data, std::move(m_recv_data));
            }
        });
    });
}

void EtcpClientImpl::real_connect()
{
    m_socket = asio::ip::tcp::socket(m_io_context, asio::ip::tcp::v4());
    m_socket.async_connect(m_endpoint, [this](const std::error_code & err){
        m_closed = false;

        if (err)
        {
            on_error("connect", err.message().c_str());
            on_close();
        }
        else
        {
            std::error_code ec;
            m_socket.set_option(asio::ip::tcp::no_delay(true), ec);
            m_socket.set_option(asio::socket_base::send_buffer_size(4 * 1024 * 1024), ec);
            m_socket.set_option(asio::socket_base::receive_buffer_size(4 * 1024 * 1024), ec);

            if (!m_request.empty())
            {
                do_proxy();
                return;
            }

            do_read();

            on_connect();
        }
    });
}

void EtcpClientImpl::real_close()
{
    if (m_socket.is_open())
    {
        std::error_code ec;
        m_socket.shutdown(asio::socket_base::shutdown_both, ec);
        m_socket.close(ec);
    }
}

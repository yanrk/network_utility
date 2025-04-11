/********************************************************
 * Description : tcp forward server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include <chrono>
#include "tcp_forward_server_impl.h"
#include "base.h"

TcpForwardServerImpl::TcpForwardServerImpl()
    : m_running(false)
    , m_random()
    , m_gateway_host()
    , m_gateway_port()
    , m_gateway_endpoint()
    , m_io_context()
    , m_work()
    , m_work_thread()
    , m_acceptor_map()
    , m_acceptor_mutex()
{

}

TcpForwardServerImpl::~TcpForwardServerImpl()
{
    exit();
}

bool TcpForwardServerImpl::init(const std::string & gateway_host, uint16_t gateway_port)
{
    exit();

    do
    {
        RUN_LOG_DBG("tcp forward server init begin");

        m_gateway_host = gateway_host;
        m_gateway_port = gateway_port;

        asio::ip::tcp::resolver resolver(m_io_context);
        asio::ip::tcp::resolver::query query(gateway_host, std::to_string(gateway_port));

        std::error_code err;
        asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, err);
        if (err)
        {
            RUN_LOG_ERR("tcp forward server init failure while resolve (%s:%u) error (%s)", gateway_host.c_str(), gateway_port, err.message().c_str());
            break;
        }

        asio::ip::tcp::resolver::iterator end;
        while (end != iter)
        {
            if (iter->endpoint().address().is_v4())
            {
                m_gateway_endpoint = iter->endpoint();
                break;
            }
            ++iter;
        }

        if (end == iter)
        {
            RUN_LOG_ERR("tcp forward server init failure while address (%s:%u) not support ipv4", gateway_host.c_str(), gateway_port);
            break;
        }

        m_running = true;

        m_random = std::knuth_b(static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count()));

        m_work = std::make_unique<asio::io_service::work>(m_io_context);
        if (!m_work)
        {
            RUN_LOG_ERR("tcp forward server init failure while create io context work failed");
            break;
        }

        m_work_thread = std::thread([this]{ m_io_context.run(); });
        if (!m_work_thread.joinable())
        {
            RUN_LOG_ERR("tcp forward server init failure while create work thread failed");
            break;
        }

        RUN_LOG_DBG("tcp forward server init success");

        return true;
    } while (false);

    RUN_LOG_ERR("tcp forward server init failure");

    exit();

    return false;
}

void TcpForwardServerImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("tcp forward server exit begin");

        m_running = false;

        m_work.reset();
        m_io_context.stop();

        if (m_work_thread.joinable())
        {
            m_work_thread.join();
        }

        release_all_local_forward_ports();

        RUN_LOG_DBG("tcp forward server exit end");
    }
}

uint16_t TcpForwardServerImpl::acquire_local_forward_port(const std::string & target_host, uint16_t target_port)
{
    if (target_host.empty() || 0 == target_port)
    {
        RUN_LOG_ERR("tcp forward server acquire port failure while invalid parameters");
        return 0;
    }

    uint32_t addr = inet_addr(target_host.c_str());
    if (INADDR_NONE == addr)
    {
        RUN_LOG_ERR("tcp forward server acquire port failure while target host (%s) is invalid ipv4", target_host.c_str());
        return 0;
    }

    uint16_t port = htons(target_port);

    std::unique_ptr<TcpForwardAcceptor> acceptor = std::make_unique<TcpForwardAcceptor>(m_io_context, m_gateway_endpoint, asio::ip::tcp::endpoint(asio::ip::make_address_v4(target_host), target_port), make_forward_message(addr, port));
    if (!acceptor)
    {
        RUN_LOG_ERR("tcp forward server acquire port failure while create tcp forward acceptor failed");
        return 0;
    }

    if (!acceptor->start())
    {
        RUN_LOG_ERR("tcp forward server acquire port failure while start tcp forward acceptor failed");
        return 0;
    }

    uint16_t listen_port = acceptor->get_listen_port();
    if (0 == listen_port)
    {
        RUN_LOG_ERR("tcp forward server acquire port failure while listen tcp forward acceptor failed");
        return 0;
    }

    {
        std::lock_guard<std::mutex> locker(m_acceptor_mutex);
        m_acceptor_map[listen_port] = std::move(acceptor);
    }

    return listen_port;
}

void TcpForwardServerImpl::release_local_forward_port(uint16_t listen_port)
{
    std::lock_guard<std::mutex> locker(m_acceptor_mutex);
    m_acceptor_map.erase(listen_port);
}

void TcpForwardServerImpl::release_all_local_forward_ports()
{
    std::lock_guard<std::mutex> locker(m_acceptor_mutex);
    m_acceptor_map.clear();
}

std::vector<uint8_t> TcpForwardServerImpl::make_forward_message(uint32_t addr, uint16_t port)
{
    uint8_t xor_val = 0x0;
    do
    {
        xor_val = static_cast<uint8_t>(m_random() % 256);
    } while (0x0 == xor_val || 'C' == xor_val);

    std::vector<uint8_t> message(sizeof(xor_val) + sizeof(addr) + sizeof(port));
    memcpy(&message[0], &xor_val, sizeof(xor_val));
    memcpy(&message[sizeof(xor_val)], &addr, sizeof(addr));
    memcpy(&message[sizeof(xor_val) + sizeof(addr)], &port, sizeof(port));

    for (size_t index = sizeof(xor_val); index < message.size(); ++index)
    {
        message[index] ^= xor_val;
    }

    return message;
}

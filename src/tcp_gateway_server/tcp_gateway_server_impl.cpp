/********************************************************
 * Description : tcp gateway server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2025
 ********************************************************/

#include <vector>
#include "tcp_gateway_server_impl.h"
#include "tcp_gateway_session.h"
#include "base.h"

TcpGatewayServerImpl::TcpGatewayServerImpl()
    : m_running(false)
    , m_acceptor()
    , m_io_works()
    , m_io_contexts()
    , m_io_context_iter(m_io_contexts.end())
    , m_work_threads()
{

}

TcpGatewayServerImpl::~TcpGatewayServerImpl()
{
    exit();
}

bool TcpGatewayServerImpl::init(const std::string & host, uint16_t port, uint32_t thread_count)
{
    exit();

    RUN_LOG_DBG("tcp gateway server init begin");

    do
    {
        if (host.empty() || 0 == port)
        {
            RUN_LOG_ERR("tcp gateway server init failure while invalid parameters");
            break;
        }

        if (thread_count < 1)
        {
            thread_count = 1;
        }
        else if (thread_count > 20)
        {
            thread_count = 20;
        }

        m_running = true;

        try
        {
            for (uint32_t index = 0; index < thread_count; ++index)
            {
                m_io_contexts.emplace_back(std::move(std::make_unique<asio::io_service>()));
                asio::io_context & io_context = *m_io_contexts.back();
                m_io_works.emplace_back(std::move(std::make_unique<asio::io_service::work>(io_context)));
                m_work_threads.emplace_back(std::move(std::make_unique<std::thread>(&TcpGatewayServerImpl::run, this, &io_context)));
            }

            m_io_context_iter = m_io_contexts.begin();

            bool reuse_address = true;
            asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::from_string(host), port);
            m_acceptor = std::make_unique<asio::ip::tcp::acceptor>(get_io_context(), endpoint, reuse_address);

            do_accept();
        }
        catch (std::error_code & err)
        {
            RUN_LOG_ERR("tcp gateway server init failure while init exception (%s)", err.message().c_str());
            break;
        }
        catch (...)
        {
            RUN_LOG_ERR("tcp gateway server init failure while init exception");
            break;
        }

        RUN_LOG_DBG("tcp gateway server init success");

        return true;
    } while (false);

    RUN_LOG_ERR("tcp gateway server init failure");

    exit();

    return false;
}

void TcpGatewayServerImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("tcp gateway server exit begin");

        m_running = false;

        m_acceptor.reset();

        m_io_works.clear();

        for (std::list<std::unique_ptr<asio::io_context>>::iterator iter = m_io_contexts.begin(); m_io_contexts.end() != iter; ++iter)
        {
            asio::io_context & io_context = **iter;
            io_context.stop();
        }

        for (std::list<std::unique_ptr<std::thread>>::iterator iter = m_work_threads.begin(); m_work_threads.end() != iter; ++iter)
        {
            std::thread & thread = **iter;
            if (thread.joinable())
            {
                thread.join();
            }
        }

        m_work_threads.clear();

        m_io_contexts.clear();

        RUN_LOG_DBG("tcp gateway server exit end");
    }
}

void TcpGatewayServerImpl::do_accept()
{
    if (!m_acceptor)
    {
        return;
    }

    asio::io_context & io_context = get_io_context();

    std::shared_ptr<asio::ip::tcp::socket> peer_socket = std::make_shared<asio::ip::tcp::socket>(io_context);
    if (!peer_socket)
    {
        RUN_LOG_ERR("tcp gateway server create peer socket failed");
        return;
    }

    std::shared_ptr<asio::ip::tcp::socket> host_socket = std::make_shared<asio::ip::tcp::socket>(io_context);
    if (!host_socket)
    {
        RUN_LOG_ERR("tcp gateway server create host socket failed");
        return;
    }

    m_acceptor->async_accept(*peer_socket, [=](const std::error_code & err){
        if (err)
        {
            if (m_running)
            {
                RUN_LOG_ERR("tcp gateway server async accept error (%s)", err.message().c_str());
            }
            return;
        }

        do_recv_head(peer_socket, host_socket);

        do_accept();
    });
}

void TcpGatewayServerImpl::do_recv_head(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket)
{
    const uint32_t request_size = 7;
    std::shared_ptr<char> request(new char[request_size], std::default_delete<char[]>());

    asio::async_read(*peer_socket, asio::buffer(request.get(), request_size), asio::transfer_exactly(request_size), [=](std::error_code err, std::size_t len){
        if (err)
        {
            RUN_LOG_ERR("tcp gateway server async read error (%s)", err.message().c_str());
            return;
        }

        char * request_data = request.get();
        if (0 == strncmp(request_data, "CONNECT", request_size))
        {
            do_recv_body(peer_socket, host_socket);
            return;
        }

        uint8_t xor_val = 0;
        memcpy(&xor_val, request_data, sizeof(xor_val));

        for (size_t index = sizeof(xor_val); index < request_size; ++index)
        {
            request_data[index] ^= xor_val;
        }

        uint32_t addr = 0;
        uint16_t port = 0;
        memcpy(&addr, request_data + sizeof(xor_val), sizeof(addr));
        memcpy(&port, request_data + sizeof(xor_val) + sizeof(addr), sizeof(port));

        std::shared_ptr<char> response = std::make_shared<char>(0 == addr || 0 == port ? 0x0 : 0x1);
        const uint32_t response_size = static_cast<uint32_t>(sizeof(uint8_t));
        std::shared_ptr<bool> result = std::make_shared<bool>(0 != *response);

        std::shared_ptr<asio::ip::tcp::endpoint> endpoint = std::make_shared<asio::ip::tcp::endpoint>(asio::ip::address_v4(ntohl(addr)), ntohs(port));

        do_send_result(peer_socket, host_socket, response, response_size, result, endpoint);
    });
}

void TcpGatewayServerImpl::do_recv_body(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket)
{
    std::shared_ptr<asio::streambuf> request = std::make_shared<asio::streambuf>();
    request->prepare(1024);

    asio::async_read_until(*peer_socket, *request, "\r\n\r\n", [=](std::error_code err, std::size_t len){
        if (err)
        {
            RUN_LOG_ERR("tcp gateway server async read until error (%s)", err.message().c_str());
            return;
        }

        const char * request_data = reinterpret_cast<const char *>(request->data().data());

        const char * host_beg = request_data;
        while (' ' == *host_beg)
        {
            ++host_beg;
        }

        const char * host_end = nullptr;
        const char * port_end = host_beg;
        while (' ' != *port_end && '\r' != *port_end)
        {
            if (':' == *port_end)
            {
                host_end = port_end;
            }
            ++port_end;
        }

        std::shared_ptr<bool> result = std::make_shared<bool>(false);
        std::shared_ptr<asio::ip::tcp::endpoint> endpoint;
        const char * response_data = nullptr;

        do
        {
            if (nullptr == host_end)
            {
                RUN_LOG_ERR("tcp gateway server resolve address exception (%s)", "invalid format");
                response_data = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
                break;
            }

            std::string host(host_beg, host_end);
            std::string port(host_end + 1, port_end);

            request->consume(len);

            asio::ip::tcp::resolver resolver(peer_socket->get_executor());
            asio::ip::tcp::resolver::query query(host, port);

            std::error_code err;
            asio::ip::tcp::resolver::iterator iter = resolver.resolve(query, err);
            if (err)
            {
                RUN_LOG_ERR("tcp gateway server resolve (%s:%s) error (%s)", host.c_str(), port.c_str(), err.message().c_str());
                response_data = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
                break;
            }

            asio::ip::tcp::resolver::iterator end;
            while (end != iter)
            {
                if (iter->endpoint().address().is_v4())
                {
                    break;
                }
                ++iter;
            }

            if (end == iter)
            {
                RUN_LOG_ERR("tcp gateway server address (%s:%s) not support ipv4", host.c_str(), port.c_str());
                response_data = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
                break;
            }

            response_data = "HTTP/1.1 200 Connection Established\r\nContent-Length: 0\r\n\r\n";
            endpoint = std::make_shared<asio::ip::tcp::endpoint>(iter->endpoint());
            *result = true;
        } while (false);

        uint32_t response_size = strlen(response_data);
        std::shared_ptr<char> response(new char[response_size], std::default_delete<char[]>());
        memcpy(response.get(), response_data, response_size);

        do_send_result(peer_socket, host_socket, response, response_size, result, endpoint);
    });
}

void TcpGatewayServerImpl::do_send_result(std::shared_ptr<asio::ip::tcp::socket> peer_socket, std::shared_ptr<asio::ip::tcp::socket> host_socket, std::shared_ptr<char> response, uint32_t response_size, std::shared_ptr<bool> result, std::shared_ptr<asio::ip::tcp::endpoint> endpoint)
{
    asio::async_write(*peer_socket, asio::buffer(response.get(), response_size), asio::transfer_all(), [=](std::error_code err, std::size_t len){
        if (err)
        {
            RUN_LOG_ERR("tcp gateway server async write error (%s)", err.message().c_str());
            return;
        }

        if (!*result)
        {
            return;
        }

        host_socket->async_connect(*endpoint, [=](std::error_code err){
            if (err)
            {
                RUN_LOG_ERR("tcp gateway server async connect error (%s)", err.message().c_str());
                return;
            }

            std::string source_host;
            uint16_t source_port = 0;
            get_address(peer_socket->remote_endpoint(err), source_host, source_port);

            std::string target_host;
            uint16_t target_port = 0;
            get_address(host_socket->remote_endpoint(err), target_host, target_port);

            std::shared_ptr<TcpGatewaySession> session = std::make_shared<TcpGatewaySession>(std::move(*host_socket), std::move(*peer_socket), target_host, target_port, source_host, source_port);
            if (!session)
            {
                RUN_LOG_ERR("tcp gateway server create tcp gateway session failed");
                return;
            }

            RUN_LOG_DBG("tcp forward: (%s:%u) <--> (%s:%u)", source_host.c_str(), source_port, target_host.c_str(), target_port);

            session->start();
        });
    });
}

void TcpGatewayServerImpl::get_address(const asio::ip::tcp::endpoint & endpoint, std::string & host, uint16_t & port)
{
    std::error_code err;
    host = endpoint.address().to_string(err);
    port = endpoint.port();
}

asio::io_context & TcpGatewayServerImpl::get_io_context()
{
    if (m_io_contexts.end() == m_io_context_iter)
    {
        m_io_context_iter = m_io_contexts.begin();
    }
    return **m_io_context_iter++;
}

void TcpGatewayServerImpl::run(asio::io_context * io_context)
{
    if (nullptr != io_context)
    {
        io_context->run();
    }
}

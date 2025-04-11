/********************************************************
 * Description : easy tcp server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "etcp_server_impl.h"

EtcpServerImpl::EtcpServerImpl()
    : m_running(false)
    , m_sink(nullptr)
    , m_acceptor()
    , m_io_works()
    , m_io_contexts()
    , m_io_context_iter(m_io_contexts.end())
    , m_work_threads()
    , m_session_map()
    , m_session_mutex()
{

}

EtcpServerImpl::~EtcpServerImpl()
{
    exit();
}

bool EtcpServerImpl::init(EtcpServerSink * sink, const std::string & host, uint16_t port, uint32_t thread_count)
{
    exit();

    RUN_LOG_DBG("etcp server init begin");

    do
    {
        if (host.empty() || 0 == port)
        {
            RUN_LOG_ERR("etcp server init failure while invalid parameters");
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
        m_sink = sink;

        try
        {
            for (uint32_t index = 0; index < thread_count; ++index)
            {
                m_io_contexts.emplace_back(std::move(std::make_unique<asio::io_service>()));
                asio::io_context & io_context = *m_io_contexts.back();
                m_io_works.emplace_back(std::move(std::make_unique<asio::io_service::work>(io_context)));
                m_work_threads.emplace_back(std::move(std::make_unique<std::thread>(&EtcpServerImpl::run, this, &io_context)));
            }

            m_io_context_iter = m_io_contexts.begin();

            bool reuse_address = true;
            asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::from_string(host), port);
            m_acceptor = std::make_unique<asio::ip::tcp::acceptor>(get_io_context(), endpoint, reuse_address);

            do_accept();
        }
        catch (std::error_code & err)
        {
            RUN_LOG_ERR("etcp server init failure while init exception (%s)", err.message().c_str());
            break;
        }
        catch (...)
        {
            RUN_LOG_ERR("etcp server init failure while init exception");
            break;
        }

        RUN_LOG_DBG("etcp server init success");

        return true;
    } while (false);

    RUN_LOG_ERR("etcp server init failure");

    exit();

    return false;
}

void EtcpServerImpl::exit()
{
    if (m_running)
    {
        RUN_LOG_DBG("etcp server exit begin");

        m_running = false;

        m_acceptor.reset();

        clear_sessions();

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

        RUN_LOG_DBG("etcp server exit end");
    }
}

void EtcpServerImpl::append_session(EtcpConnectionSharedPtr session)
{
    if (!!session)
    {
        std::lock_guard<std::mutex> locker(m_session_mutex);
        m_session_map[session.get()] = session;
    }
}

void EtcpServerImpl::remove_session(EtcpConnectionSharedPtr session)
{
    if (!!session)
    {
        std::lock_guard<std::mutex> locker(m_session_mutex);
        m_session_map.erase(session.get());
    }
}

void EtcpServerImpl::clear_sessions()
{
    {
        std::lock_guard<std::mutex> locker(m_session_mutex);

        for (std::map<void *, EtcpConnectionWeakPtr>::iterator iter = m_session_map.begin(); m_session_map.end() != iter; ++iter)
        {
            EtcpConnectionSharedPtr session = iter->second.lock();
            if (!!session)
            {
                session->close();
            }
        }
    }

    while (true)
    {
        {
            std::lock_guard<std::mutex> locker(m_session_mutex);

            if (m_session_map.empty())
            {
                break;
            }
        }
        sleep_ms(5);
    }
}

void EtcpServerImpl::do_accept()
{
    if (!m_acceptor)
    {
        return;
    }

    std::shared_ptr<EtcpSession> session = std::make_shared<EtcpSession>(m_sink, *this, get_io_context());
    m_acceptor->async_accept(session->get_socket(), [this, session](const std::error_code & err)
    {
        if (!err)
        {
            session->start();
        }

        do_accept();
    });
}

asio::io_context & EtcpServerImpl::get_io_context()
{
    if (m_io_contexts.end() == m_io_context_iter)
    {
        m_io_context_iter = m_io_contexts.begin();
    }
    return **m_io_context_iter++;
}

void EtcpServerImpl::run(asio::io_context * io_context)
{
    if (nullptr != io_context)
    {
        io_context->run();
    }
}

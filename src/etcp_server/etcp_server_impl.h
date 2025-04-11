/********************************************************
 * Description : easy tcp server implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef ETCP_SERVER_IMPL_H
#define ETCP_SERVER_IMPL_H


#include <map>
#include <list>
#include <string>
#include <thread>
#include <mutex>
#include "asio.hpp"
#include "etcp_server.h"
#include "etcp_session.h"
#include "base.h"

class EtcpServerImpl
{
public:
    EtcpServerImpl();
    ~EtcpServerImpl();

public:
    bool init(EtcpServerSink * sink, const std::string & host, uint16_t port, uint32_t thread_count);
    void exit();

public:
    void append_session(EtcpConnectionSharedPtr session);
    void remove_session(EtcpConnectionSharedPtr session);
    void clear_sessions();

private:
    void do_accept();

private:
    asio::io_context & get_io_context();

private:
    void run(asio::io_context * io_context);

private:
    bool                                                    m_running;
    EtcpServerSink                                        * m_sink;
    std::unique_ptr<asio::ip::tcp::acceptor>                m_acceptor;
    std::list<std::unique_ptr<asio::io_context::work>>      m_io_works;
    std::list<std::unique_ptr<asio::io_context>>            m_io_contexts;
    std::list<std::unique_ptr<asio::io_context>>::iterator  m_io_context_iter;
    std::list<std::unique_ptr<std::thread>>                 m_work_threads;

private:
    std::map<void *, EtcpConnectionWeakPtr>                 m_session_map;
    std::mutex                                              m_session_mutex;
};


#endif // ETCP_SERVER_IMPL_H

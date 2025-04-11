/********************************************************
 * Description : multicast helper implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef MULTICAST_HELPER_IMPL_H
#define MULTICAST_HELPER_IMPL_H


#include <list>
#include <array>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "asio.hpp"
#include "multicast_helper.h"

class MulticastHelperImpl
{
public:
    MulticastHelperImpl();
    virtual ~MulticastHelperImpl();

public:
    bool init(MulticastRecver * multicast_recver, const std::string & multicast_group_host, uint16_t multicast_group_port, const std::string & protocol_prefix, const std::string & protocol_suffix);
    void exit();

public:
    bool send_data(const std::string & data);

private:
    void handle_data();
    void context_run(bool send_not_recv);

private:
    void do_recv();
    void do_send();

private:
    bool packet_message(const std::string & data, std::string & message);
    bool unpack_message(const std::string & message, std::string & data);
    bool handle_message(const std::string & address, const std::string & message);

private:
    bool                                                    m_running;
    std::string                                             m_protocol_prefix;
    std::string                                             m_protocol_suffix;
    MulticastRecver                                       * m_multicast_recver;
    std::string                                             m_multicast_group_host;
    uint16_t                                                m_multicast_group_port;

private:
    asio::ip::udp::endpoint                                 m_multicast_endpoint;
    asio::ip::udp::endpoint                                 m_peer_endpoint;

private:
    asio::io_context                                        m_send_context;
    asio::ip::udp::socket                                   m_send_socket;

private:
    asio::io_context                                        m_recv_context;
    asio::ip::udp::socket                                   m_recv_socket;

private:
    std::list<std::string>                                  m_send_data_list;

private:
    std::array<char, 1472>                                  m_recv_buffer;
    std::list<std::pair<std::string, std::string>>          m_recv_data_list;
    std::mutex                                              m_recv_data_mutex;
    std::condition_variable                                 m_recv_data_condition;

private:
    std::thread                                             m_send_run_thread;
    std::thread                                             m_recv_run_thread;
    std::thread                                             m_recv_handle_thread;
};


#endif // MULTICAST_HELPER_IMPL_H

/********************************************************
 * Description : enet client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#ifndef ENET_CLIENT_IMPL_H
#define ENET_CLIENT_IMPL_H


#include <list>
#include <mutex>
#include <thread>
#include <string>
#include <vector>
#include <condition_variable>

extern "C"
{
    #include "enet.h"
}

#include "enet_client.h"

class EnetClientImpl
{
public:
    EnetClientImpl(const void * user_data);
    ~EnetClientImpl();

public:
    bool init(EnetClientSink * sink, const char * host, uint16_t port);
    void exit();

public:
    void connect();
    void close();
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    void on_connect();
    void on_close();
    void do_connect();
    void do_close();

private:
    bool                                                    m_running;
    const void                                            * m_user_data;
    EnetClientSink                                        * m_sink;
    std::string                                             m_host;
    uint16_t                                                m_port;
    ENetHost                                              * m_enet_host;
    ENetPeer                                              * m_enet_peer;

private:
    std::list<std::string>                                  m_send_data_list;
    std::mutex                                              m_send_data_mutex;
    std::thread                                             m_send_data_thread;

private:
    std::list<bool>                                         m_event_list;
    std::mutex                                              m_event_mutex;
    std::condition_variable                                 m_event_condition;
    std::thread                                             m_event_thread;

private:
    std::list<bool>                                         m_callback_list;
    std::mutex                                              m_callback_mutex;
    std::condition_variable                                 m_callback_condition;
    std::thread                                             m_callback_thread;
};


#endif // ENET_CLIENT_IMPL_H

/********************************************************
 * Description : easy tcp client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef ETCP_CLIENT_IMPL_H
#define ETCP_CLIENT_IMPL_H


#include <list>
#include <vector>
#include <string>
#include <thread>
#include "asio.hpp"
#include "etcp_client.h"
#include "base.h"

#pragma pack(1)

struct EtcpHeader
{
    uint16_t    sign;
    uint32_t    size;

    void encode()
    {
        host_to_net(&sign, sizeof(sign));
        host_to_net(&size, sizeof(size));
    }

    void decode()
    {
        net_to_host(&sign, sizeof(sign));
        net_to_host(&size, sizeof(size));
    }
};

#pragma pack()

class EtcpClientImpl
{
public:
    EtcpClientImpl(const void * user_data);
    ~EtcpClientImpl();

public:
    bool init(EtcpClientSink * sink, const std::string & target_host, uint16_t target_port, const std::string & gateway_host, uint16_t gateway_port, const std::string & connect_data);
    void exit();

public:
    void connect();
    void close();
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    void on_error(const char * action, const char * message);
    void on_connect();
    void on_close();

private:
    void do_connect();
    void do_close();
    void do_proxy();
    void do_write();
    void do_read();

private:
    void real_connect();
    void real_close();

private:
    bool                                                    m_running;
    bool                                                    m_closed;
    const void                                            * m_user_data;
    EtcpClientSink                                        * m_sink;
    std::string                                             m_request;
    asio::streambuf                                         m_response;
    asio::ip::tcp::endpoint                                 m_endpoint;
    asio::io_context                                        m_io_context;
    std::unique_ptr<asio::io_context::work>                 m_work;
    asio::ip::tcp::socket                                   m_socket;
    uint8_t                                                 m_recv_head[sizeof(EtcpHeader)];
    std::string                                             m_recv_data;
    std::list<std::string>                                  m_send_data_list;
    std::thread                                             m_work_thread;
};


#endif // ETCP_CLIENT_IMPL_H

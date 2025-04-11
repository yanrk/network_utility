/********************************************************
 * Description : easy tcp session
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef ETCP_SESSION_H
#define ETCP_SESSION_H


#include <list>
#include <vector>
#include <string>
#include <thread>
#include "asio.hpp"
#include "etcp_server.h"
#include "base.h"

#pragma pack(1)

struct EtcpPrefix
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

class EtcpSession : public EtcpConnection, public std::enable_shared_from_this<EtcpSession>
{
public:
    EtcpSession(EtcpServerSink * sink, EtcpServerImpl & server, asio::io_context & io_context);
    ~EtcpSession();

public:
    asio::ip::tcp::socket & get_socket();
    void start();

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) override;
    virtual void get_peer_address(std::string & ip, unsigned short & port) override;

public:
    virtual void close() override;
    virtual bool send_message(const void * data, uint32_t size) override;
    virtual bool send_message(std::string && packet) override;

private:
    void on_error(const char * action, const char * message);
    void on_accept();
    void on_close();

private:
    void do_close();
    void do_write();
    void do_read();

private:
    bool                                                    m_closed;
    EtcpServerSink                                        * m_sink;
    EtcpServerImpl                                        & m_server;
    asio::io_context                                      & m_io_context;
    asio::ip::tcp::socket                                   m_socket;
    std::string                                             m_host_ip;
    uint16_t                                                m_host_port;
    std::string                                             m_peer_ip;
    uint16_t                                                m_peer_port;
    std::string                                             m_request;
    asio::streambuf                                         m_response;
    uint8_t                                                 m_recv_head[sizeof(EtcpPrefix)];
    std::string                                             m_recv_data;
    std::list<std::string>                                  m_send_data_list;
};


#endif // ETCP_SESSION_H

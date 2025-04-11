/********************************************************
 * Description : easy tcp server
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef ETCP_SERVER_H
#define ETCP_SERVER_H


#include <cstdint>
#include <string>
#include <memory>

class EtcpConnection
{
public:
    EtcpConnection();
    virtual ~EtcpConnection();

public:
    void set_user_data(void * user_data);
    void * get_user_data();

public:
    virtual void get_host_address(std::string & ip, unsigned short & port) = 0;
    virtual void get_peer_address(std::string & ip, unsigned short & port) = 0;

public:
    virtual void close() = 0;
    virtual bool send_message(const void * data, uint32_t size) = 0;
    virtual bool send_message(std::string && packet) = 0;

private:
    void                          * m_user_data;
};

typedef std::shared_ptr<EtcpConnection> EtcpConnectionSharedPtr;
typedef std::weak_ptr<EtcpConnection> EtcpConnectionWeakPtr;

struct EtcpServerSink
{
    virtual ~EtcpServerSink();
    virtual void on_etcp_accept(EtcpConnectionSharedPtr connection) = 0;
    virtual void on_etcp_close(EtcpConnectionSharedPtr connection) = 0;
    virtual void on_etcp_error(EtcpConnectionSharedPtr connection, const char * action, const char * message) = 0;
    virtual void on_etcp_recv(EtcpConnectionSharedPtr connection, std::string && packet) = 0;
};

class EtcpServerImpl;

class EtcpServer
{
public:
    EtcpServer();
    ~EtcpServer();

public:
    bool init(EtcpServerSink * sink, const char * host, uint16_t port, uint32_t thread_count = 1);
    void exit();

private:
    EtcpServer(const EtcpServer &) = delete;
    EtcpServer(EtcpServer &&) = delete;
    EtcpServer & operator = (const EtcpServer &) = delete;
    EtcpServer & operator = (EtcpServer &&) = delete;

private:
    EtcpServerImpl                * m_impl;
};


#endif // ETCP_SERVER_H

/********************************************************
 * Description : easy tcp client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef ETCP_CLIENT_H
#define ETCP_CLIENT_H


#include <cstdint>
#include <string>

struct EtcpClientSink
{
    virtual ~EtcpClientSink();
    virtual void on_etcp_connect(const void * user_data) = 0;
    virtual void on_etcp_close(const void * user_data) = 0;
    virtual void on_etcp_error(const void * user_data, const char * action, const char * message) = 0;
    virtual void on_etcp_recv(const void * user_data, std::string && packet) = 0;
};

class EtcpClientImpl;

class EtcpClient
{
public:
    EtcpClient(const void * user_data);
    ~EtcpClient();

public:
    bool init(EtcpClientSink * sink, const char * target_host, uint16_t target_port, const char * connect_data);
    bool init(EtcpClientSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data);
    void exit();

public:
    void connect();
    void close();
    bool send_message(const void * data, uint32_t size);
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    EtcpClient(const EtcpClient &) = delete;
    EtcpClient(EtcpClient &&) = delete;
    EtcpClient & operator = (const EtcpClient &) = delete;
    EtcpClient & operator = (EtcpClient &&) = delete;

private:
    EtcpClientImpl                * m_impl;
    const void                    * m_user_data;
};


#endif // ETCP_CLIENT_H

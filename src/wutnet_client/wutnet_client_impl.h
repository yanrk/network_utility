/********************************************************
 * Description : ws & udp & tcp network client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef WUTNET_CLIENT_IMPL_H
#define WUTNET_CLIENT_IMPL_H


#include "websocket_client.h"
#include "enet_client.h"
#include "etcp_client.h"
#include "wutnet_client.h"

struct WutnetBase
{
    virtual ~WutnetBase();
    virtual bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data) = 0;
    virtual void exit() = 0;
    virtual void connect() = 0;
    virtual void close() = 0;
    virtual bool send_message(std::string && packet) = 0;
    virtual bool is_connected() const = 0;
};

class WutnetOnWebsocket : public WutnetBase, public WebsocketClientSink
{
public:
    WutnetOnWebsocket(const void * user_data);
    ~WutnetOnWebsocket();

public:
    virtual bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data) override;
    virtual void exit() override;

public:
    virtual void connect() override;
    virtual void close() override;
    virtual bool send_message(std::string && packet) override;
    virtual bool is_connected() const override;

public:
    virtual void on_websocket_connect(const void * user_data) override;
    virtual void on_websocket_close(const void * user_data) override;
    virtual void on_websocket_error(const void * user_data, const char * action, const char * message) override;
    virtual void on_websocket_recv(const void * user_data, std::string && packet, bool binary) override;

private:
    bool                            m_running;
    WebsocketClient                 m_client;
    WutnetSink                    * m_sink;
    const char                    * m_name;
};

class WutnetOnEnet : public WutnetBase, public EnetClientSink
{
public:
    WutnetOnEnet(const void * user_data);
    ~WutnetOnEnet();

public:
    virtual bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data) override;
    virtual void exit() override;

public:
    virtual void connect() override;
    virtual void close() override;
    virtual bool send_message(std::string && packet) override;
    virtual bool is_connected() const override;

public:
    virtual void on_enet_connect(const void * user_data) override;
    virtual void on_enet_close(const void * user_data) override;
    virtual void on_enet_error(const void * user_data, const char * action, const char * message) override;
    virtual void on_enet_recv(const void * user_data, const void * data, uint32_t size) override;

private:
    bool                            m_running;
    EnetClient                      m_client;
    WutnetSink                    * m_sink;
    const char                    * m_name;
};

class WutnetOnEtcp : public WutnetBase, public EtcpClientSink
{
public:
    WutnetOnEtcp(const void * user_data);
    ~WutnetOnEtcp();

public:
    virtual bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data) override;
    virtual void exit() override;

public:
    virtual void connect() override;
    virtual void close() override;
    virtual bool send_message(std::string && packet) override;
    virtual bool is_connected() const override;

public:
    virtual void on_etcp_connect(const void * user_data) override;
    virtual void on_etcp_close(const void * user_data) override;
    virtual void on_etcp_error(const void * user_data, const char * action, const char * message) override;
    virtual void on_etcp_recv(const void * user_data, std::string && packet) override;

private:
    bool                            m_running;
    EtcpClient                      m_client;
    WutnetSink                    * m_sink;
    const char                    * m_name;
};

class WutnetClientImpl
{
public:
    WutnetClientImpl();
    ~WutnetClientImpl();

public:
    bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data, wutnet_type_t wutnet_type, const void * user_data);
    void exit();

public:
    void connect();
    void close();
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    WutnetClientImpl(const WutnetClientImpl &) = delete;
    WutnetClientImpl(WutnetClientImpl &&) = delete;
    WutnetClientImpl & operator = (const WutnetClientImpl &) = delete;
    WutnetClientImpl & operator = (WutnetClientImpl &&) = delete;

private:
    WutnetBase                    * m_wutnet;
};


#endif // WUTNET_CLIENT_IMPL_H

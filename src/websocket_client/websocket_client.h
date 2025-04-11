/********************************************************
 * Description : websocket client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H


#include <cstdint>
#include <string>

struct WebsocketClientSink
{
    virtual ~WebsocketClientSink();
    virtual void on_websocket_connect(const void * user_data) = 0;
    virtual void on_websocket_close(const void * user_data) = 0;
    virtual void on_websocket_error(const void * user_data, const char * action, const char * message) = 0;
    virtual void on_websocket_recv(const void * user_data, std::string && packet, bool binary) = 0;
};

class WebsocketSessionBase;

class WebsocketClient
{
public:
    WebsocketClient(const void * user_data);
    ~WebsocketClient();

public:
    bool init(WebsocketClientSink * sink, const char * target_url);
    bool init(WebsocketClientSink * sink, const char * target_url, const char * gateway_url);
    bool init(WebsocketClientSink * sink, const char * target_host, uint16_t target_port, bool target_secure);
    bool init(WebsocketClientSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data);
    void exit();

public:
    void connect();
    void close();
    bool send_message(const void * data, uint32_t size, bool binary);
    bool send_message(std::string && packet, bool binary);
    bool is_connected() const;

private:
    WebsocketClient(const WebsocketClient &) = delete;
    WebsocketClient(WebsocketClient &&) = delete;
    WebsocketClient & operator = (const WebsocketClient &) = delete;
    WebsocketClient & operator = (WebsocketClient &&) = delete;

private:
    WebsocketSessionBase          * m_session;
    const void                    * m_user_data;
};


#endif // WEBSOCKET_CLIENT_H

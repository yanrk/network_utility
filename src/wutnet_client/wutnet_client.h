/********************************************************
 * Description : ws & udp & tcp network client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef WUTNET_CLIENT_H
#define WUTNET_CLIENT_H


#include <cstdint>
#include <string>
#include "macros.h"

enum class wutnet_type_t : uint8_t
{
    wutnet_on_ws,
    wutnet_on_enet,
    wutnet_on_etcp
};

struct GOOFER_API WutnetSink
{
    virtual ~WutnetSink();
    virtual void on_connect(const char * protocol, const void * user_data) = 0;
    virtual void on_close(const char * protocol, const void * user_data) = 0;
    virtual void on_error(const char * protocol, const void * user_data, const char * action, const char * message) = 0;
    virtual void on_recv(const char * protocol, const void * user_data, std::string && packet) = 0;
};

class WutnetClientImpl;

class GOOFER_API WutnetClient
{
public:
    WutnetClient();
    ~WutnetClient();

public:
    bool init(WutnetSink * sink, const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data, wutnet_type_t wutnet_type, const void * user_data);
    void exit();

public:
    void connect();
    void close();
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    WutnetClient(const WutnetClient &) = delete;
    WutnetClient(WutnetClient &&) = delete;
    WutnetClient & operator = (const WutnetClient &) = delete;
    WutnetClient & operator = (WutnetClient &&) = delete;

private:
    WutnetClientImpl              * m_impl;
};


#endif // WUTNET_CLIENT_H

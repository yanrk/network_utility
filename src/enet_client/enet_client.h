/********************************************************
 * Description : enet client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#ifndef ENET_CLIENT_H
#define ENET_CLIENT_H


#include <cstdint>
#include <string>
#include "macros.h"

struct GOOFER_API EnetClientSink
{
    virtual ~EnetClientSink();
    virtual void on_enet_connect(const void * user_data) = 0;
    virtual void on_enet_close(const void * user_data) = 0;
    virtual void on_enet_error(const void * user_data, const char * action, const char * message) = 0;
    virtual void on_enet_recv(const void * user_data, const void * data, uint32_t size) = 0;
};

class EnetClientImpl;

class GOOFER_API EnetClient
{
public:
    EnetClient(const void * user_data);
    ~EnetClient();

public:
    bool init(EnetClientSink * sink, const char * host, uint16_t port);
    void exit();

public:
    void connect();
    void close();
    bool send_message(const void * data, uint32_t size);
    bool send_message(std::string && packet);
    bool is_connected() const;

private:
    EnetClient(const EnetClient &) = delete;
    EnetClient(EnetClient &&) = delete;
    EnetClient & operator = (const EnetClient &) = delete;
    EnetClient & operator = (EnetClient &&) = delete;

private:
    EnetClientImpl                * m_impl;
    const void                    * m_user_data;
};


#endif // ENET_CLIENT_H

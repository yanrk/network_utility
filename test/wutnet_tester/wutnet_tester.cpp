/********************************************************
 * Description : sample of wutnet client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "wutnet_client.h"

struct WutnetTester : public WutnetSink
{
    WutnetTester()
        : m_running(false)
        , m_wutnet_client()
    {

    }

    virtual ~WutnetTester()
    {
        exit();
    }

    bool init(const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port, const char * connect_data, wutnet_type_t wutnet_type)
    {
        exit();

        m_running = true;

        if (!m_wutnet_client.init(this, target_host, target_port, gateway_host, gateway_port, connect_data, wutnet_type, nullptr))
        {
            printf("wutnet client init failed\n");
            return false;
        }

        m_wutnet_client.connect();

        return true;
    }

    void exit()
    {
        if (!m_running)
        {
            return;
        }

        m_running = false;
        m_wutnet_client.exit();
    }

    virtual void on_connect(const char * protocol, const void * user_data) override
    {
        printf("wutnet client connect success on %s\n", protocol);
    }

    virtual void on_close(const char * protocol, const void * user_data) override
    {
        printf("wutnet client close on %s\n", protocol);
    }

    virtual void on_error(const char * protocol, const void * user_data, const char * action, const char * message) override
    {
        printf("wutnet client error on %s, action: %s, message: %s\n", protocol, action, message);
    }

    virtual void on_recv(const char * protocol, const void * user_data, std::string && packet) override
    {
        printf("wutnet client recv on %s, size: %zu\n", protocol, packet.size());
    }

    bool            m_running;
    WutnetClient    m_wutnet_client;
};

int main(int argc, char * argv[])
{
    if (argc < 6)
    {
        printf("usage: %s protocol target_host target_port gateway_host gateway_port [connect_data]\n", argv[0]);
        return 1;
    }

    wutnet_type_t wutnet_type;
    if (0 == strcmp(argv[1], "ws"))
    {
        wutnet_type = wutnet_type_t::wutnet_on_ws;
    }
    else if (0 == strcmp(argv[1], "enet"))
    {
        wutnet_type = wutnet_type_t::wutnet_on_enet;
    }
    else if (0 == strcmp(argv[1], "etcp"))
    {
        wutnet_type = wutnet_type_t::wutnet_on_etcp;
    }
    else
    {
        printf("only support protocols: ws enet etcp\n");
        return 2;
    }

    const char * target_host = argv[2];
    uint16_t target_port = static_cast<uint16_t>(atoi(argv[3]));
    const char * gateway_host = argv[4];
    uint16_t gateway_port = static_cast<uint16_t>(atoi(argv[5]));
    const char * connect_data = (argc > 6) ? argv[6] : nullptr;

    printf("wutnet tester init begin\n");

    WutnetTester wutnet_tester;
    if (!wutnet_tester.init(target_host, target_port, gateway_host, gateway_port, connect_data, wutnet_type))
    {
        printf("wutnet tester init failed\n");
        return 5;
    }

    printf("wutnet tester init success\n");

    for (int i = 0; i < 3; ++i)
    {
        getchar();
    }

    printf("wutnet tester exit begin\n");

    wutnet_tester.exit();

    printf("wutnet tester exit end\n");

    return 0;
}

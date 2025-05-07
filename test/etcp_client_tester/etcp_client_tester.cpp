/********************************************************
 * Description : sample of etcp client tester
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "etcp_client.h"

class EtcpClientTester : public EtcpClientSink
{
public:
    EtcpClientTester()
        : m_client(nullptr)
    {

    }

    virtual ~EtcpClientTester()
    {
        exit();
    }

public:
    bool init(const char * target_host, uint16_t target_port, const char * gateway_host, uint16_t gateway_port)
    {
        if (!m_client.init(this, target_host, target_port, gateway_host, gateway_port, nullptr))
        {
            return false;
        }

        m_client.connect();

        return true;
    }

    void exit()
    {
        m_client.exit();
    }

public:
    virtual void on_etcp_connect(const void * user_data) override
    {
        printf("connect\n");
        if (m_client.send_message("hello", 5))
        {
            printf("send data [hello]\n");
        }
    }

    virtual void on_etcp_close(const void * user_data) override
    {
        printf("disconnect\n");
    }

    virtual void on_etcp_error(const void * user_data, const char * action, const char * message) override
    {
        printf("(%s) error (%s)\n", action, message);
    }

    virtual void on_etcp_recv(const void * user_data, std::string && packet) override
    {
        printf("recv data [%s]\n", packet.c_str());
    }

private:
    EtcpClient m_client;
};

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        printf("usage: %s target_host target_port [gateway_host] [gateway_port]\n", argv[0]);
        return 1;
    }

    const char * target_host = (argv[1]);
    uint16_t target_port = static_cast<uint16_t>(atoi(argv[2]));
    const char * gateway_host = (argc > 4 ? argv[3] : nullptr);
    uint16_t gateway_port = (argc > 4 ? static_cast<uint16_t>(atoi(argv[4])) : 0);

    EtcpClientTester tester;
    if (!tester.init(target_host, target_port, gateway_host, gateway_port))
    {
        return 2;
    }

    for (int i = 0; i < 3; ++i)
    {
        getchar();
    }

    tester.exit();

    return 0;
}

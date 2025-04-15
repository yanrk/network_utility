/********************************************************
 * Description : sample of etcp server tester
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "etcp_server.h"

class EtcpServerTester : public EtcpServerSink
{
public:
    EtcpServerTester()
        : m_server()
    {

    }

    virtual ~EtcpServerTester()
    {
        exit();
    }

public:
    bool init(const char * host, uint16_t port)
    {
        return m_server.init(this, host, port);
    }

    void exit()
    {
        m_server.exit();
    }

public:
    virtual void on_etcp_accept(EtcpConnectionSharedPtr connection) override
    {
        printf("accept\n");
    }

    virtual void on_etcp_close(EtcpConnectionSharedPtr connection) override
    {
        printf("disconnect\n");
    }

    virtual void on_etcp_error(EtcpConnectionSharedPtr connection, const char * action, const char * message) override
    {
        printf("(%s) error (%s)\n", action, message);
    }

    virtual void on_etcp_recv(EtcpConnectionSharedPtr connection, std::string && packet) override
    {
        printf("recv data [%s]\n", packet.c_str());
        if (connection->send_message(std::move(packet)))
        {
            printf("send back\n");
        }
    }

private:
    EtcpServer m_server;
};

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        printf("usage: %s host port\n", argv[0]);
        return 1;
    }

    const char * host = (argv[1]);
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    EtcpServerTester tester;
    if (!tester.init(host, port))
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

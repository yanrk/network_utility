/********************************************************
 * Description : sample of tcp gateway server tester
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tcp_gateway_server.h"

int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        printf("usage: %s gateway_host gateway_port\n", argv[0]);
        return 1;
    }

    const char * gateway_host = (argv[1]);
    uint16_t gateway_port = static_cast<uint16_t>(atoi(argv[2]));

    TcpGatewayServer server;
    if (!server.init(gateway_host, gateway_port, 1))
    {
        return 2;
    }

    for (int i = 0; i < 3; ++i)
    {
        getchar();
    }

    server.exit();

    return 0;
}

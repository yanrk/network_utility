/********************************************************
 * Description : sample of tcp forward server tester
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "tcp_forward_server.h"

int main(int argc, char * argv[])
{
    if (argc < 5)
    {
        printf("usage: %s gateway_host gateway_port target_host target_port\n", argv[0]);
        return 1;
    }

    const char * gateway_host = (argv[1]);
    uint16_t gateway_port = static_cast<uint16_t>(atoi(argv[2]));
    const char * target_host = (argv[3]);
    uint16_t target_port = static_cast<uint16_t>(atoi(argv[4]));

    TcpForwardServer server;
    if (!server.init(gateway_host, gateway_port))
    {
        return 2;
    }

    uint16_t forward_port = server.acquire_local_forward_port(target_host, target_port);
    if (0 == forward_port)
    {
        return 3;
    }

    printf("get forward port: %u\n", forward_port);

    for (int i = 0; i < 3; ++i)
    {
        getchar();
    }

    server.release_local_forward_port(forward_port);

    server.exit();

    return 0;
}

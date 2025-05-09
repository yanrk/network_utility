/********************************************************
 * Description : sample of multicast helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "multicast_helper.h"
#include "base.h"

#define MULTICAST_GROUP_HOST "239.0.0.1"
#define MULTICAST_GROUP_PORT 6789
#define MULTICAST_GROUP_PREFIX "TEST<"
#define MULTICAST_GROUP_SUFFIX ">TEST"
#define MULTICAST_GROUP_DATA "this is a multicast test data"

struct MulticastAccepter : public MulticastRecver
{
    virtual void recv_multicast_data(const std::string & multicast_group_host, uint16_t multicast_group_port, const std::string & address, const std::string & data)
    {
        assert(MULTICAST_GROUP_HOST == multicast_group_host);
        assert(MULTICAST_GROUP_PORT == multicast_group_port);
        assert(MULTICAST_GROUP_DATA == data);
        printf("recv multicast data, multicast: (%s:%u), address: (%s), data: (%s)\n", multicast_group_host.c_str(), multicast_group_port, address.c_str(), data.c_str());
    }
};

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        printf("usage: %s need_send\n", argv[0]);
        return 1;
    }

    bool need_send = (0 != strcmp(argv[1], "0") && 0 != strcmp(argv[1], "false"));

    MulticastAccepter multicast_accepter;

    MulticastHelper multicast_helper;
    if (!multicast_helper.init(&multicast_accepter, MULTICAST_GROUP_HOST, MULTICAST_GROUP_PORT, MULTICAST_GROUP_PREFIX, MULTICAST_GROUP_SUFFIX))
    {
        printf("init multicast helper failed\n");
        return 2;
    }

    if (need_send)
    {
        multicast_helper.send_multicast_data(MULTICAST_GROUP_DATA);
        printf("send multicast data, multicast: (%s:%u), data: (%s)\n", MULTICAST_GROUP_HOST, MULTICAST_GROUP_PORT, MULTICAST_GROUP_DATA);
    }

    for (int i = 0; i < 3; ++i)
    {
        getchar();
    }

    multicast_helper.exit();

    return 0;
}

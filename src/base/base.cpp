/********************************************************
 * Description : base
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <cstdarg>
#include <cstdlib>
#include "base.h"

uint64_t get_ns_time()
{
    struct timespec time = {0, 0};
    clock_gettime(CLOCK_MONOTONIC, &time);
    return static_cast<uint64_t>(time.tv_sec) * 1000000000 + static_cast<uint64_t>(time.tv_nsec);
}

uint64_t get_ms_time()
{
    return get_ns_time() / 1000000;
}

uint64_t get_s_time()
{
    return get_ns_time() / 1000000000;
}

void sleep_ms(uint32_t ms)
{
    usleep(1000 * ms);
}

static int s_log_level_min = 1;

void run_log(int level, const char * file, const char * func, int line, const char * format, ...)
{
    if (level < s_log_level_min)
    {
        return;
    }
    char buffer[1024] = { 0x0 };
    size_t size = 0;
#ifndef DEBUG
    size = snprintf(buffer, sizeof(buffer) - 1, "%s:%s:%d | ", file, func, line);
#endif // DEBUG
    va_list args;
    va_start(args, format);
    vsnprintf(buffer + size, sizeof(buffer) - 1 - size, format, args);
    va_end(args);
    printf("%s\n", buffer);
}

void order_bytes(void * obj, size_t size)
{
    static unsigned short us = 0x0001;
    static unsigned char * uc = reinterpret_cast<unsigned char *>(&us);

    if (size > 1 && 0x01 == uc[0])
    {
        unsigned char * bytes = static_cast<unsigned char *>(obj);
        for (size_t i = 0; i < size / 2; ++i)
        {
            unsigned char temp = bytes[i];
            bytes[i] = bytes[size - 1 - i];
            bytes[size - 1 - i] = temp;
        }
    }
}

void host_to_net(void * obj, size_t size)
{
    order_bytes(obj, size);
}

void net_to_host(void * obj, size_t size)
{
    order_bytes(obj, size);
}

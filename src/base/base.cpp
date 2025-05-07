/********************************************************
 * Description : base
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "macros.h"

#ifdef GOOFER_OS_IS_WIN
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <sys/timeb.h>
#else
    #include <sys/time.h>
    #include <unistd.h>
    #include <errno.h>
#endif // GOOFER_OS_IS_WIN

#include <sys/stat.h>
#include <ctime>
#include <cstdarg>
#include <cstdlib>
#include "base.h"

uint64_t get_ns_time()
{
#ifdef GOOFER_OS_IS_WIN
    static bool s_got_clock_frequency = false;
    static LARGE_INTEGER s_clock_frequency;
    if (!s_got_clock_frequency)
    {
        QueryPerformanceFrequency(&s_clock_frequency);
        s_got_clock_frequency = true;
    }
    LARGE_INTEGER clock_count;
    QueryPerformanceCounter(&clock_count);
    double current_time = static_cast<double>(clock_count.QuadPart);
    current_time *= 1000000000.0;
    current_time /= static_cast<double>(s_clock_frequency.QuadPart);
    return static_cast<uint64_t>(current_time);
#elif defined(GOOFER_OS_IS_MAC)
    return static_cast<uint64_t>(clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW));
#else
    struct timespec ts = { 0x0 };
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * GOOFER_U64_VAL(1000000000) + static_cast<uint64_t>(ts.tv_nsec);
#endif // GOOFER_OS_IS_WIN
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
#ifdef GOOFER_OS_IS_WIN
    Sleep(ms);
#else
    usleep(1000 * ms);
#endif // GOOFER_OS_IS_WIN
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

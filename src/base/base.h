/********************************************************
 * Description : base
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef BASE_H
#define BASE_H


#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include "macros.h"

#define ORDER_BYTES(x) order_bytes(&x, sizeof(x))
#define HOST_TO_NET(x) host_to_net(&x, sizeof(x))
#define NET_TO_HOST(x) net_to_host(&x, sizeof(x))

#define RUN_LOG_TRK(fmt, ...) run_log(0, __FILENAME__, __FUNCTION__, __LINE__, "[DBG] " fmt, ##__VA_ARGS__)
#define RUN_LOG_DBG(fmt, ...) run_log(1, __FILENAME__, __FUNCTION__, __LINE__, "[DBG] " fmt, ##__VA_ARGS__)
#define RUN_LOG_WAR(fmt, ...) run_log(2, __FILENAME__, __FUNCTION__, __LINE__, "[WAR] " fmt, ##__VA_ARGS__)
#define RUN_LOG_ERR(fmt, ...) run_log(3, __FILENAME__, __FUNCTION__, __LINE__, "[ERR] " fmt, ##__VA_ARGS__)
#define RUN_LOG_CRI(fmt, ...) run_log(4, __FILENAME__, __FUNCTION__, __LINE__, "[CRI] " fmt, ##__VA_ARGS__)

GOOFER_CXX_API(void) run_log(int level, const char * file, const char * func, int line, const char * format, ...);

GOOFER_CXX_API(uint64_t) get_ns_time();
GOOFER_CXX_API(uint64_t) get_ms_time();
GOOFER_CXX_API(uint64_t) get_s_time();
GOOFER_CXX_API(void) sleep_ms(uint32_t ms);
GOOFER_CXX_API(void) order_bytes(void * obj, size_t size);
GOOFER_CXX_API(void) host_to_net(void * obj, size_t size);
GOOFER_CXX_API(void) net_to_host(void * obj, size_t size);


#endif // BASE_H

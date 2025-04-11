/********************************************************
 * Description : multicast helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef MULTICAST_HELPER_H
#define MULTICAST_HELPER_H


#include <cstdint>
#include <string>

struct MulticastRecver
{
    ~MulticastRecver();
    virtual void accept_shared_device(const std::string & multicast_group_host, uint16_t multicast_group_port, const std::string & address, const std::string & data) = 0;
};

class MulticastHelperImpl;

class MulticastHelper
{
public:
    MulticastHelper();
    ~MulticastHelper();

public:
    bool init(MulticastRecver * multicast_recver, const std::string & multicast_group_host, uint16_t multicast_group_port, std::string protocol_prefix = "", std::string protocol_suffix = "");
    void exit();

public:
    bool send_data(const std::string & data);

private:
    MulticastHelper(const MulticastHelper &) = delete;
    MulticastHelper(MulticastHelper &&) = delete;
    MulticastHelper & operator = (const MulticastHelper &) = delete;
    MulticastHelper & operator = (MulticastHelper &&) = delete;

private:
    MulticastHelperImpl           * m_impl;
};


#endif // MULTICAST_HELPER_H

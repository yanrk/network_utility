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
#include "macros.h"

/*--------------------------------------------------------------------------------------
 |                         The Class D range is split like this                        |
 --------------------------------------------------------------------------------------|
 |                       Description                     |             Range           |
 |-------------------------------------------------------|-----------------------------|
 | Reserved for special (well-known) multicast addresses | 224.0.0.0 - 224.0.0.255     |
 | Globally-scoped (Internet-wide) multicast addresses   | 224.0.1.0 - 238.255.255.255 |
 | Administratively-scoped (local) multicast addresses   | 239.0.0.0 - 239.255.255.255 |
 --------------------------------------------------------------------------------------*/

struct GOOFER_API MulticastRecver
{
    ~MulticastRecver();
    virtual void recv_multicast_data(const std::string & multicast_group_host, uint16_t multicast_group_port, const std::string & address, const std::string & data) = 0;
};

class MulticastHelperImpl;

class GOOFER_API MulticastHelper
{
public:
    MulticastHelper();
    ~MulticastHelper();

public:
    bool init(MulticastRecver * multicast_recver, const std::string & multicast_group_host, uint16_t multicast_group_port, std::string protocol_prefix = "", std::string protocol_suffix = "");
    void exit();

public:
    bool send_multicast_data(const std::string & data);

private:
    MulticastHelper(const MulticastHelper &) = delete;
    MulticastHelper(MulticastHelper &&) = delete;
    MulticastHelper & operator = (const MulticastHelper &) = delete;
    MulticastHelper & operator = (MulticastHelper &&) = delete;

private:
    MulticastHelperImpl           * m_impl;
};


#endif // MULTICAST_HELPER_H

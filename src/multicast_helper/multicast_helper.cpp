/********************************************************
 * Description : multicast helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include "multicast_helper.h"
#include "multicast_helper_impl.h"

MulticastRecver::~MulticastRecver()
{

}

MulticastHelper::MulticastHelper()
    : m_impl(nullptr)
{

}

MulticastHelper::~MulticastHelper()
{
    exit();
}

bool MulticastHelper::init(MulticastRecver * multicast_recver, const std::string & multicast_group_host, uint16_t multicast_group_port, std::string protocol_prefix, std::string protocol_suffix)
{
    exit();

    do
    {
        m_impl = new MulticastHelperImpl;
        if (nullptr == m_impl)
        {
            break;
        }

        if (!m_impl->init(multicast_recver, multicast_group_host, multicast_group_port, protocol_prefix, protocol_suffix))
        {
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

void MulticastHelper::exit()
{
    if (nullptr != m_impl)
    {
        m_impl->exit();
        delete m_impl;
        m_impl = nullptr;
    }
}

bool MulticastHelper::send_data(const std::string & data)
{
    return nullptr != m_impl && m_impl->send_data(data);
}

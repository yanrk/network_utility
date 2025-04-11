/********************************************************
 * Description : websocket client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstring>
#include <string>
#include "websocket_client.h"
#include "websocket_client_impl.h"
#include "base.h"

WebsocketClientSink::~WebsocketClientSink()
{

}

WebsocketClient::WebsocketClient(const void * user_data)
    : m_session(nullptr)
    , m_user_data(user_data)
{

}

WebsocketClient::~WebsocketClient()
{
    exit();
}

bool WebsocketClient::init(WebsocketClientSink * sink, const char * target_url)
{
    return init(sink, target_url, "");
}

bool WebsocketClient::init(WebsocketClientSink * sink, const char * target_url, const char * gateway_url)
{
    exit();

    RUN_LOG_DBG("websocket client init begin");

    if (nullptr == target_url)
    {
        target_url = "";
    }

    if (nullptr == gateway_url)
    {
        gateway_url = "";
    }

    if (0x0 == *target_url)
    {
        if (0x0 == *gateway_url)
        {
            RUN_LOG_ERR("websocket client init failure while invalid parameters");
            return false;
        }
        std::swap(target_url, gateway_url);
    }

    if (0 == strncmp(target_url, "wss", 3))
    {
        m_session = new WebsocketSessionSecure(m_user_data);
    }
    else
    {
        m_session = new WebsocketSessionPlain(m_user_data);
    }

    if (nullptr == m_session)
    {
        RUN_LOG_ERR("websocket client init failure while create websocket session");
        return false;
    }

    if (m_session->init(sink, target_url, gateway_url))
    {
        RUN_LOG_DBG("websocket client init success");
        return true;
    }

    RUN_LOG_ERR("websocket client init failure");

    exit();

    return false;
}

bool WebsocketClient::init(WebsocketClientSink * sink, const char * target_host, uint16_t target_port, bool target_secure)
{
    std::string target_url;
    if (nullptr != target_host && 0x0 != *target_host && 0 != target_port)
    {
        target_url = std::string(target_secure ? "wss://" : "ws://") + target_host + ":" + std::to_string(target_port);
    }
    return init(sink, target_url.c_str(), "");
}

bool WebsocketClient::init(WebsocketClientSink * sink, const char * target_host, uint16_t target_port, bool target_secure, const char * gateway_host, uint16_t gateway_port, bool gateway_secure, const char * connect_data)
{
    std::string target_url;
    if (nullptr != target_host && 0x0 != *target_host && 0 != target_port)
    {
        target_url = std::string(target_secure ? "wss://" : "ws://") + target_host + ":" + std::to_string(target_port);
    }
    std::string gateway_url;
    if (nullptr != gateway_host && 0x0 != *gateway_host && 0 != gateway_port)
    {
        gateway_url = std::string(gateway_secure ? "wss://" : "ws://") + gateway_host + ":" + std::to_string(gateway_port);
        if (target_url.empty() && nullptr != connect_data && 0x0 != *connect_data)
        {
            gateway_url += "/?token=";
            gateway_url += connect_data;
        }
    }
    return init(sink, target_url.c_str(), gateway_url.c_str());
}

void WebsocketClient::exit()
{
    if (nullptr != m_session)
    {
        RUN_LOG_DBG("websocket client exit begin");

        m_session->exit();
        delete m_session;
        m_session = nullptr;

        RUN_LOG_DBG("websocket client exit end");
    }
}

void WebsocketClient::connect()
{
    if (nullptr != m_session)
    {
        m_session->connect();
    }
}

void WebsocketClient::close()
{
    if (nullptr != m_session)
    {
        m_session->close();
    }
}

bool WebsocketClient::send_message(const void * data, uint32_t size, bool binary)
{
    return nullptr != m_session && m_session->send_message(data, size, binary);
}

bool WebsocketClient::send_message(std::string && packet, bool binary)
{
    return nullptr != m_session && m_session->send_message(packet.data(), static_cast<uint32_t>(packet.size()), binary);
}

bool WebsocketClient::is_connected() const
{
    return nullptr != m_session && m_session->is_connected();
}

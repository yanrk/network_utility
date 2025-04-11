/********************************************************
 * Description : websocket client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef WEBSOCKET_CLIENT_IMPL_H
#define WEBSOCKET_CLIENT_IMPL_H


#include <list>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif
#define _WEBSOCKETPP_NULLPTR_
#define _WEBSOCKETPP_INITIALIZER_LISTS_
#ifndef _WEBSOCKETPP_CPP11_STL_
#define _WEBSOCKETPP_CPP11_STL_
#endif
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#define _WEBSOCKETPP_CPP11_MEMORY_
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_

#include "websocketpp/config/asio_client.hpp"
#include "websocketpp/client.hpp"

#include "websocket_client.h"
#include "base.h"

class WebsocketSessionBase
{
public:
    WebsocketSessionBase();
    virtual ~WebsocketSessionBase();

public:
    virtual bool init(WebsocketClientSink * sink, const std::string & target_url, const std::string & gateway_url) = 0;
    virtual void exit() = 0;

public:
    virtual void connect() = 0;
    virtual void close() = 0;
    virtual bool send_message(const void * data, uint32_t size, bool binary) = 0;
    virtual bool is_connected() const = 0;
};

template <typename client_type>
class WebsocketSession : public WebsocketSessionBase
{
public:
    WebsocketSession(const void * user_data);
    virtual ~WebsocketSession();

public:
    virtual bool init(WebsocketClientSink * sink, const std::string & target_url, const std::string & gateway_url) override;
    virtual void exit() override;

public:
    virtual void connect() override;
    virtual void close() override;
    virtual bool send_message(const void * data, uint32_t size, bool binary) override;
    virtual bool is_connected() const override;

protected:
    websocketpp::client<client_type> & get_client();
    void set_options(asio::ip::tcp::socket & socket);
    void on_error(const char * action, const char * message);

private:
    void on_connect();
    void on_close();

private:
    void do_connect();
    void do_close();

private:
    void real_connect();
    void real_close();

private:
    virtual void set_specific_handler() = 0;

private:
    typedef typename websocketpp::client<client_type>       websocket_client_t;
    typedef typename websocket_client_t::connection_ptr     websocket_connection_shared_ptr_t;

private:
    bool                                                    m_running;
    bool                                                    m_callback;
    const void                                            * m_user_data;
    WebsocketClientSink                                   * m_sink;
    std::string                                             m_target_url;
    std::string                                             m_gateway_url;
    websocket_client_t                                      m_client;
    websocket_connection_shared_ptr_t                       m_connection;
    std::thread                                             m_work_thread;

private:
    std::list<bool>                                         m_event_list;
    std::mutex                                              m_event_mutex;
    std::condition_variable                                 m_event_condition;
    std::thread                                             m_event_thread;

private:
    std::list<bool>                                         m_callback_list;
    std::mutex                                              m_callback_mutex;
    std::condition_variable                                 m_callback_condition;
    std::thread                                             m_callback_thread;

private:
    uint64_t                                                m_recv_pong_time;
    std::thread                                             m_pingpong_thread;
};

template <typename client_type>
WebsocketSession<client_type>::WebsocketSession(const void * user_data)
    : m_running(false)
    , m_callback(false)
    , m_user_data(user_data)
    , m_sink(nullptr)
    , m_target_url()
    , m_gateway_url()
    , m_client()
    , m_connection()
    , m_work_thread()
    , m_event_list()
    , m_event_mutex()
    , m_event_condition()
    , m_event_thread()
    , m_callback_list()
    , m_callback_mutex()
    , m_callback_condition()
    , m_callback_thread()
    , m_recv_pong_time(0)
    , m_pingpong_thread()
{
    m_client.clear_access_channels(websocketpp::log::alevel::all);
    m_client.clear_error_channels(websocketpp::log::elevel::all);
    m_client.init_asio();
}

template <typename client_type>
WebsocketSession<client_type>::~WebsocketSession()
{
    exit();
}

template <typename client_type>
bool WebsocketSession<client_type>::init(WebsocketClientSink * sink, const std::string & target_url, const std::string & gateway_url)
{
    exit();

    if (target_url.size() < 2)
    {
        RUN_LOG_ERR("websocket client init failure while invalid paramenters");
        return false;
    }

    if (gateway_url.empty() || target_url == gateway_url)
    {
        m_target_url = target_url;
        m_gateway_url = target_url;
    }
    else
    {
        if (target_url.substr(0, 2) == "ws")
        {
            m_target_url = "http" + target_url.substr(2);
        }
        else
        {
            m_target_url = target_url;
        }
        m_gateway_url = gateway_url;
    }

    m_sink = sink;

    m_client.set_close_handler([this](websocketpp::connection_hdl handle){
        if (handle.lock() != m_connection)
        {
            return;
        }
        m_connection.reset();
        on_close();
    });

    m_client.set_fail_handler([this](websocketpp::connection_hdl handle){
        if (handle.lock() != m_connection)
        {
            return;
        }
        m_connection.reset();
        on_close();
    });

    m_client.set_http_handler([this](websocketpp::connection_hdl handle){

    });

    m_client.set_interrupt_handler([this](websocketpp::connection_hdl handle){
        if (handle.lock() != m_connection)
        {
            return;
        }
        m_connection.reset();
        on_close();
    });

    m_client.set_message_handler([this](websocketpp::connection_hdl handle, websocketpp::config::asio_client::message_type::ptr message){
        if (nullptr != message)
        {
            std::string & packet = message->get_raw_payload();
            bool binary = websocketpp::frame::opcode::BINARY == message->get_opcode();
            if (nullptr != m_sink && !packet.empty())
            {
                m_sink->on_websocket_recv(m_user_data, std::move(packet), binary);
            }
        }
    });

    m_client.set_open_handler([this](websocketpp::connection_hdl handle){
        if (handle.lock() != m_connection)
        {
            return;
        }
        on_connect();
    });

    m_client.set_pong_handler([this](websocketpp::connection_hdl handle, std::string message){
        m_recv_pong_time = get_ms_time();
    });

    m_client.set_tcp_post_init_handler([this](websocketpp::connection_hdl handle){

    });

    m_client.set_tcp_pre_init_handler([this](websocketpp::connection_hdl handle){

    });

    m_client.set_validate_handler([this](websocketpp::connection_hdl handle){
        return true;
    });

    set_specific_handler();

    m_running = true;

    m_event_thread = std::thread([this]{
        while (m_running)
        {
            std::list<bool> event_list;

            {
                std::unique_lock<std::mutex> locker(m_event_mutex);
                while (m_running && m_event_list.empty())
                {
                    m_event_condition.wait(locker);
                }
                event_list.swap(m_event_list);
            }

            for (std::list<bool>::const_iterator iter = event_list.begin(); event_list.end() != iter && m_running; ++iter)
            {
                if (*iter)
                {
                    do_connect();
                }
                else
                {
                    do_close();
                }
            }
        }
    });

    m_callback_thread = std::thread([this]{
        while (m_running)
        {
            std::list<bool> callback_list;

            {
                std::unique_lock<std::mutex> locker(m_callback_mutex);
                while (m_running && m_callback_list.empty())
                {
                    m_callback_condition.wait(locker);
                }
                callback_list.swap(m_callback_list);
            }

            for (std::list<bool>::const_iterator iter = callback_list.begin(); callback_list.end() != iter && m_running; ++iter)
            {
                if (nullptr == m_sink)
                {
                    continue;
                }
                if (*iter)
                {
                    m_sink->on_websocket_connect(m_user_data);
                }
                else
                {
                    m_sink->on_websocket_close(m_user_data);
                }
            }
        }
    });

    m_pingpong_thread = std::thread([this]{
        uint64_t send_ping_time = 0;
        uint64_t run_close_time = 0;
        while (m_running)
        {
            websocket_connection_shared_ptr_t connection = m_connection;
            if (!connection || websocketpp::session::state::value::open != connection->get_state())
            {
                send_ping_time = 0;
                run_close_time = 0;
                sleep_ms(5);
                continue;
            }

            uint64_t current_time = get_ms_time();
            if (current_time < send_ping_time + 500 || current_time < run_close_time + 3000)
            {
                sleep_ms(5);
            }
            else if (send_ping_time > m_recv_pong_time + 5000)
            {
                run_close_time = current_time;
                close();
            }
            else
            {
                if (0 == send_ping_time)
                {
                    m_recv_pong_time = current_time;
                }
                send_ping_time = current_time;
                connection->ping("ping");
            }
        }
    });

    do
    {
        if (!m_event_thread.joinable())
        {
            RUN_LOG_ERR("websocket client init failure while create event thread failed");
            break;
        }

        if (!m_callback_thread.joinable())
        {
            RUN_LOG_ERR("websocket client init failure while create callback thread failed");
            break;
        }

        if (!m_pingpong_thread.joinable())
        {
            RUN_LOG_ERR("websocket client init failure while create pingpong thread failed");
            break;
        }

        return true;
    } while (false);

    exit();

    return false;
}

template <typename client_type>
void WebsocketSession<client_type>::exit()
{
    if (m_running)
    {
        m_running = false;

        do_close();

        if (m_event_thread.joinable())
        {
            m_event_condition.notify_one();
            m_event_thread.join();
        }

        if (m_callback_thread.joinable())
        {
            m_callback_condition.notify_one();
            m_callback_thread.join();
        }

        if (m_pingpong_thread.joinable())
        {
            m_pingpong_thread.join();
        }

        if (m_work_thread.joinable())
        {
            m_work_thread.join();
        }

        m_event_list.clear();
        m_callback_list.clear();

        m_callback = false;
    }
}

template <typename client_type>
void WebsocketSession<client_type>::connect()
{
    {
        std::lock_guard<std::mutex> locker(m_event_mutex);
        m_event_list.push_back(true);
    }
    m_event_condition.notify_one();
}

template <typename client_type>
void WebsocketSession<client_type>::close()
{
    {
        std::lock_guard<std::mutex> locker(m_event_mutex);
        m_event_list.push_back(false);
    }
    m_event_condition.notify_one();
}

template <typename client_type>
void WebsocketSession<client_type>::on_connect()
{
    {
        std::lock_guard<std::mutex> locker(m_callback_mutex);
        m_callback_list.push_back(true);
    }
    m_callback_condition.notify_one();
}

template <typename client_type>
void WebsocketSession<client_type>::on_close()
{
    if (!m_callback)
    {
        return;
    }

    m_callback = false;

    {
        std::lock_guard<std::mutex> locker(m_callback_mutex);
        m_callback_list.push_back(false);
    }
    m_callback_condition.notify_one();
}

template <typename client_type>
void WebsocketSession<client_type>::on_error(const char * action, const char * message)
{
    if (nullptr != m_sink)
    {
        m_sink->on_websocket_error(m_user_data, action, message);
    }
}

template <typename client_type>
void WebsocketSession<client_type>::do_connect()
{
    if (m_running)
    {
        if (is_connected())
        {
            real_close();

            if (m_work_thread.joinable())
            {
                m_work_thread.join();
            }

            on_close();
        }
        else
        {
            if (m_work_thread.joinable())
            {
                m_work_thread.join();
            }
        }

        m_work_thread = std::thread([this]{
            m_callback = true;
            real_connect();
            m_client.run();
        });
    }
    else
    {
        m_callback = true;
        on_close();
    }
}

template <typename client_type>
void WebsocketSession<client_type>::do_close()
{
    if (is_connected())
    {
        real_close();

        if (m_work_thread.joinable())
        {
            m_work_thread.join();
        }

        on_close();
    }
}

template <typename client_type>
bool WebsocketSession<client_type>::send_message(const void * data, uint32_t size, bool binary)
{
    websocket_connection_shared_ptr_t connection = m_connection;
    if (!m_running || !connection || nullptr == data || 0 == size)
    {
        return false;
    }

    websocketpp::lib::error_code err = connection->send(data, size, binary ? websocketpp::frame::opcode::BINARY : websocketpp::frame::opcode::TEXT);
    if (err)
    {
        on_error("send", err.message().c_str());
        return false;
    }

    return true;
}

template <typename client_type>
bool WebsocketSession<client_type>::is_connected() const
{
    websocket_connection_shared_ptr_t connection = m_connection;
    return connection && websocketpp::session::state::value::open == connection->get_state();
}

template <typename client_type>
websocketpp::client<client_type> & WebsocketSession<client_type>::get_client()
{
    return m_client;
}

template <typename client_type>
void WebsocketSession<client_type>::set_options(asio::ip::tcp::socket & socket)
{
    std::error_code err;

    asio::ip::tcp::no_delay option_no_delay(true);
    asio::socket_base::send_buffer_size option_send_buffer_size(1024 * 256);
    asio::socket_base::receive_buffer_size option_receive_buffer_size(1024 * 256);

    socket.set_option(option_no_delay, err);
    socket.set_option(option_send_buffer_size, err);
    socket.set_option(option_receive_buffer_size, err);
}

template <typename client_type>
void WebsocketSession<client_type>::real_connect()
{
    try
    {
        if (m_client.stopped())
        {
            m_client.reset();
        }
        websocketpp::lib::error_code err;
        websocket_connection_shared_ptr_t conn = m_client.get_connection(m_gateway_url, err);
        if (err)
        {
            on_error("connect", err.message().c_str());
        }
        else
        {
            if (conn)
            {
                if (m_target_url != m_gateway_url)
                {
                    conn->replace_header("Origin", m_target_url);
                }
                m_connection = m_client.connect(conn);
                if (m_connection)
                {
                    return;
                }
            }
            else
            {
                on_error("connect", "unable to resolve address");
            }
        }
    }
    catch (websocketpp::lib::error_code err)
    {
        on_error("connect", err.message().c_str());
    }
    catch (const std::exception & e)
    {
        on_error("connect", e.what());
    }
    catch (...)
    {
        on_error("connect", "an unknown exception occurred");
    }

    on_close();
}

template <typename client_type>
void WebsocketSession<client_type>::real_close()
{
    try
    {
        if (m_connection)
        {
            websocketpp::lib::error_code err;
            m_connection->terminate(err);
            m_connection.reset();
        }
        m_client.stop();
    }
    catch (websocketpp::lib::error_code err)
    {
        on_error("close", err.message().c_str());
    }
    catch (const std::exception & e)
    {
        on_error("close", e.what());
    }
    catch (...)
    {
        on_error("close", "an unknown exception occurred");
    }
}

class WebsocketSessionPlain : public WebsocketSession<websocketpp::config::asio_client>
{
public:
    WebsocketSessionPlain(const void * user_data);

private:
    virtual void set_specific_handler() override;
};

class WebsocketSessionSecure : public WebsocketSession<websocketpp::config::asio_tls_client>
{
public:
    WebsocketSessionSecure(const void * user_data);

private:
    virtual void set_specific_handler() override;
};


#endif // WEBSOCKET_CLIENT_IMPL_H

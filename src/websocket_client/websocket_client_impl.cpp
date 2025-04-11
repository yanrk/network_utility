/********************************************************
 * Description : websocket client implement
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 2.0
 * Copyright(C): 2024
 ********************************************************/

#include "websocket_client_impl.h"

WebsocketSessionBase::WebsocketSessionBase()
{

}

WebsocketSessionBase::~WebsocketSessionBase()
{

}

WebsocketSessionPlain::WebsocketSessionPlain(const void * user_data)
    : WebsocketSession<websocketpp::config::asio_client>(user_data)
{

}

void WebsocketSessionPlain::set_specific_handler()
{
    get_client().set_socket_init_handler([this](websocketpp::connection_hdl handle, asio::ip::tcp::socket & socket){
        set_options(socket);
    });
}

WebsocketSessionSecure::WebsocketSessionSecure(const void * user_data)
    : WebsocketSession<websocketpp::config::asio_tls_client>(user_data)
{

}

void WebsocketSessionSecure::set_specific_handler()
{
    get_client().set_socket_init_handler([this](websocketpp::connection_hdl handle, asio::ssl::stream<asio::ip::tcp::socket> & socket){
        set_options(socket.next_layer());
    });

    get_client().set_tls_init_handler([this](websocketpp::connection_hdl handle){
        std::shared_ptr<asio::ssl::context> ctx = std::make_shared<asio::ssl::context>(asio::ssl::context::sslv23);
        try
        {
            ctx->set_options(asio::ssl::context::default_workarounds | asio::ssl::context::no_sslv2 | asio::ssl::context::no_sslv3 | asio::ssl::context::single_dh_use);
        }
        catch (std::exception & e)
        {
            on_error("tls init", e.what());
        }
        return ctx;
    });
}

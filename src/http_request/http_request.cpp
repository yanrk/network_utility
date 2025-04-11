/********************************************************
 * Description : http request
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#include "http_request.h"

HttpRequest::HttpRequest()
{

}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::do_work()
{
    send_request();
}
/*
void HttpRequest::build_request(std::string & request);
{

}

void HttpRequest::parse_response(const std::string & response)
{

}

void HttpRequest::send_request()
{
    std::string request;
    build_request(request);
    HttpClient http_client;
    http_client.post(request, std::bind(&HttpRequest::handle_response, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpRequest::handle_response(bool result, const std::string & response)
{
    if (result)
    {
        parse_response(response);
    }
}
*/

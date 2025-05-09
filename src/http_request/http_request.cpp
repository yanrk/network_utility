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

void HttpRequest::parse_response(const std::string & response, std::string & error)
{

}

void HttpRequest::send_request()
{
    std::string url("https://example.com/");
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::string request;
    build_request(request);
    HttpClient http_client;
    http_client.post(scheme, target, request, std::bind(&HttpRequest::handle_response, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpRequest::handle_response(bool result, const std::string & response)
{
    if (result)
    {
        std::string error;
        parse_response(response, error);
    }
}
*/

/********************************************************
 * Description : http request
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * History     :
 * Copyright(C): 2024
 ********************************************************/

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H


#include <string>

class HttpRequest
{
public:
    HttpRequest();
    virtual ~HttpRequest();

public:
    void do_work();

private:
    virtual void build_request(std::string & request) = 0;
    virtual void parse_response(const std::string & response, std::string & err_code) = 0;
    virtual void send_request() = 0;
    virtual void handle_response(bool result, const std::string & response) = 0;
};


#endif // HTTP_REQUEST_H

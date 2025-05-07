/********************************************************
 * Description : sample of http client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstdio>
#include "http_client.h"
#include "http_request.h"
#include "json_helper.h"
#include "base.h"

#define SERVER "echo-http-requests.appspot.com"
#define TARGET "/echo"

class HttpRequestEcho1 : public HttpRequest
{
public:
    HttpRequestEcho1()
        : m_done(true)
        , m_req()
        , m_rsp()
    {

    }

    virtual ~HttpRequestEcho1()
    {

    }

public:
    bool done() const
    {
        return m_done;
    }

private:
    struct sample_1_t
    {
        std::string         a;
        int                 b;
    };

private:
    virtual void build_request(std::string & request) override
    {
        m_req.a = "abc";
        m_req.b = 123;

        json_t json;
        build_element(json, "a", m_req.a);
        build_element(json, "b", m_req.b);

        json_to_string(json, request);
    }

    virtual void parse_response(const std::string & response, std::string & err_code) override
    {
        json_t json_rsp;
        string_to_json(response, json_rsp);

        json_t json_req;
        parse_element(json_rsp, "request", json_req);

        std::string body;
        parse_element(json_req, "body", body);

        json_t json;
        string_to_json(body, json);

        parse_element(json, "a", m_rsp.a);
        parse_element(json, "b", m_rsp.b);

        if (m_req.a == m_rsp.a && m_req.b == m_rsp.b)
        {
            printf("echo 1 test success\n");
        }
        else
        {
            printf("echo 1 test failure\n");
        }
    }

    virtual void send_request() override
    {
        m_done = false;
        std::string request;
        build_request(request);
        HttpClient http_client;
        http_client.post(SERVER, TARGET, request, std::bind(&HttpRequestEcho1::handle_response, this, std::placeholders::_1, std::placeholders::_2));
    }

    virtual void handle_response(bool result, const std::string & response) override
    {
        if (result)
        {
            printf("http request echo 1 post success\n");
            std::string err;
            parse_response(response, err);
        }
        else
        {
            printf("http request echo 1 post failure\n");
        }
        m_done = true;
    }

private:
    bool                    m_done;
    sample_1_t              m_req;
    sample_1_t              m_rsp;
};

class HttpRequestEcho2 : public HttpRequest
{
public:
    HttpRequestEcho2()
        : m_done(true)
        , m_req()
        , m_rsp()
    {

    }

    virtual ~HttpRequestEcho2()
    {

    }

public:
    bool done() const
    {
        return m_done;
    }

private:
    struct sample_2_t
    {
        std::string         x;
        std::vector<int>    y;
    };

private:
    virtual void build_request(std::string & request) override
    {
        m_req.x = "abc";
        m_req.y = { 123, 345, 567 };

        json_t json;
        build_element(json, "x", m_req.x);
        build_element(json, "y", m_req.y);

        json_to_string(json, request);
    }

    virtual void parse_response(const std::string & response, std::string & err_code) override
    {
        json_t json_rsp;
        string_to_json(response, json_rsp);

        json_t json_req;
        parse_element(json_rsp, "request", json_req);

        std::string body;
        parse_element(json_req, "body", body);

        json_t json;
        string_to_json(body, json);

        parse_element(json, "x", m_rsp.x);
        parse_element(json, "y", m_rsp.y);

        if (m_req.x == m_rsp.x && m_req.y == m_rsp.y)
        {
            printf("echo 2 test success\n");
        }
        else
        {
            printf("echo 2 test failure\n");
        }
    }

    virtual void send_request() override
    {
        m_done = false;
        std::string request;
        build_request(request);
        HttpClient http_client;
        http_client.post(SERVER, TARGET, request, std::bind(&HttpRequestEcho2::handle_response, this, std::placeholders::_1, std::placeholders::_2));
    }

    virtual void handle_response(bool result, const std::string & response) override
    {
        if (result)
        {
            printf("http request echo 2 post success\n");
            std::string err;
            parse_response(response, err);
        }
        else
        {
            printf("http request echo 2 post failure\n");
        }
        m_done = true;
    }

private:
    bool                    m_done;
    sample_2_t              m_req;
    sample_2_t              m_rsp;
};


void test1()
{
    bool done = false;
    std::string host("baidu.com");
    std::string uri("/index.html");
    HttpClient client;
    client.get(host, uri, [&](bool result, const std::string & response){
        printf("http get request: result %s, response (%s)\n", (result ? "success" : "failure"), response.c_str());
        done = true;
    });
    while (!done)
    {
        sleep_ms(1);
    }
}

void test2()
{
    HttpRequestEcho1 echo1;
    echo1.do_work();
    while (!echo1.done())
    {
        sleep_ms(1);
    }

    HttpRequestEcho2 echo2;
    echo2.do_work();
    while (!echo2.done())
    {
        sleep_ms(1);
    }
}

int main(int argc, char * argv[])
{
    test1();
    test2();
    return 0;
}

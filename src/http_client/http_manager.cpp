/********************************************************
 * Description : http manager
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include "http_manager.h"
#include "task_actuator.hpp"
#include "base.h"

static time_t s_http_timeout = 3;

static bool http_get_request(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & request_headers, std::string & response_data)
{
    std::string url(scheme + target);

    try
    {
        httplib::Client client(scheme);

        client.set_connection_timeout(s_http_timeout);
        client.set_read_timeout(s_http_timeout);
        client.set_write_timeout(s_http_timeout);
#ifndef _MSC_VER
        client.enable_server_certificate_verification(false);
#endif // _MSC_VER

        httplib::Headers headers(request_headers.begin(), request_headers.end());
        httplib::Result result = client.Get(target, headers);
        if (result && 2 == result->status / 100)
        {
            response_data.swap(result->body);
            return true;
        }
        else
        {
            std::string err(result ? httplib::to_string(result.error()) : "send failure");
            RUN_LOG_ERR("http get request (%s) error: %s", url.c_str(), err.c_str());
        }
    }
    catch (const std::exception & e)
    {
        RUN_LOG_ERR("http get request (%s) exception: %s", url.c_str(), e.what());
    }
    catch (...)
    {
        RUN_LOG_ERR("http get request (%s) exception: unknown", url.c_str());
    }

    return false;
}

static bool http_post_request(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & request_headers, const std::string & request_data, std::string & response_data)
{
    std::string url(scheme + target);

    try
    {
        httplib::Client client(scheme);

        client.set_connection_timeout(s_http_timeout);
        client.set_read_timeout(s_http_timeout);
        client.set_write_timeout(s_http_timeout);
#ifndef _MSC_VER
        client.enable_server_certificate_verification(false);
#endif // _MSC_VER

        httplib::Headers headers(request_headers.begin(), request_headers.end());
        httplib::Result result = client.Post(target, headers, request_data, "application/json");
        if (result && 2 == result->status / 100)
        {
            response_data.swap(result->body);
            return true;
        }
        else
        {
            std::string err(result ? httplib::to_string(result.error()) : "send failure");
            RUN_LOG_ERR("http post request (%s) error: %s", url.c_str(), err.c_str());
        }
    }
    catch (const std::exception & e)
    {
        RUN_LOG_ERR("http post request (%s) exception: %s", url.c_str(), e.what());
    }
    catch (...)
    {
        RUN_LOG_ERR("http post request (%s) exception: unknown", url.c_str());
    }

    return false;
}

static bool http_upload_request(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & request_headers, const std::multimap<std::string, std::string> & request_forms, const std::string & src_file_path, const std::string & dst_file_name, std::string & response_data)
{
    std::string url(scheme + target);

    try
    {
        httplib::Client client(scheme);

        client.set_connection_timeout(s_http_timeout);
        client.set_read_timeout(s_http_timeout);
        client.set_write_timeout(s_http_timeout);
#ifndef _MSC_VER
        client.enable_server_certificate_verification(false);
#endif // _MSC_VER

        httplib::Headers headers(request_headers.begin(), request_headers.end());

        std::ifstream file(src_file_path, std::ios::binary);
        std::stringstream buffer;
        buffer << file.rdbuf();

        httplib::MultipartFormDataItems items = { { dst_file_name, buffer.str(), src_file_path, "multipart/form-data" } };
        for (std::multimap<std::string, std::string>::const_iterator iter = request_forms.begin(); request_forms.end() != iter; ++iter)
        {
            httplib::MultipartFormData form_data = { iter->first, iter->second };
            items.emplace_back(std::move(form_data));
        }

        httplib::Result result = client.Post(target, headers, items);
        if (result)
        {
            if (2 == result->status / 100)
            {
                return true;
            }
            else
            {
                std::string err(httplib::to_string(result.error()));
                RUN_LOG_ERR("http upload (%s) request (%s) error: %s", src_file_path.c_str(), url.c_str(), err.c_str());
                response_data.swap(result->body);
            }
        }
        else
        {
            RUN_LOG_ERR("http upload (%s) request (%s) error: send failure", src_file_path.c_str(), url.c_str());
        }
    }
    catch (const std::exception & e)
    {
        RUN_LOG_ERR("http upload (%s) request (%s) exception: %s", src_file_path.c_str(), url.c_str(), e.what());
    }
    catch (...)
    {
        RUN_LOG_ERR("http upload (%s) request (%s) exception: unknown", src_file_path.c_str(), url.c_str());
    }

    return false;
}

static bool http_download_request(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & request_headers, const std::string & save_file_path, std::string & response_data, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    std::string url(scheme + target);

    try
    {
        httplib::Client client(scheme);

        client.set_connection_timeout(s_http_timeout);
        client.set_read_timeout(s_http_timeout);
        client.set_write_timeout(s_http_timeout);
#ifndef _MSC_VER
        client.enable_server_certificate_verification(false);
#endif // _MSC_VER

        std::ofstream file;

        httplib::Headers headers(request_headers.begin(), request_headers.end());
        httplib::Result result = client.Get(target, headers,
            [&](const httplib::Response & response){
                if (200 == response.status)
                {
                    file.open(save_file_path, std::ios::binary | std::ios::trunc);
                    return true;
                }
                else
                {
                    return false;
                }
            },
            [&](const char * data, size_t size){
                file.write(data, size);
                return true;
            },
            [&](uint64_t current, uint64_t total){
                return progress(current, total);
            }
        );

        if (result)
        {
            if (200 == result->status)
            {
                file.close();
                return true;
            }
            else
            {
                std::string err(httplib::to_string(result.error()));
                RUN_LOG_ERR("http download (%s) request (%s) error: %s", save_file_path.c_str(), url.c_str(), err.c_str());
                response_data.swap(result->body);
            }
        }
        else
        {
            RUN_LOG_ERR("http download (%s) request (%s) error: send failure", save_file_path.c_str(), url.c_str());
        }
    }
    catch (const std::exception & e)
    {
        RUN_LOG_ERR("http download (%s) request (%s) exception: %s", save_file_path.c_str(), url.c_str(), e.what());
    }
    catch (...)
    {
        RUN_LOG_ERR("http download (%s) request (%s) exception: unknown", save_file_path.c_str(), url.c_str());
    }

    return false;
}

HttpManager::HttpManager()
    : m_task_actuator(2)
{

}

HttpManager::~HttpManager()
{

}

void HttpManager::get(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback)
{
    m_task_actuator.async_exec([=]{
        std::string response;
        bool result = http_get_request(scheme, target, headers, response);
        callback(result, response);
    });
}

void HttpManager::post(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback)
{
    m_task_actuator.async_exec([=]{
        std::string response;
        bool result = http_post_request(scheme, target, headers, request, response);
        callback(result, response);
    });
}

void HttpManager::upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    m_task_actuator.async_exec([=]{
        std::string response;
        bool result = http_upload_request(scheme, target, headers, forms, src_file_path, dst_file_name, response);
        callback(result, response);
    });
}

void HttpManager::download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    m_task_actuator.async_exec([=]{
        std::string response;
        bool result = http_download_request(scheme, target, headers, save_file_path, response, progress);
        callback(result, response);
    });
}

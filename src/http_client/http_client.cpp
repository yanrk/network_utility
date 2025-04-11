/********************************************************
 * Description : http client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "http_client.h"
#include "http_manager.h"

void HttpClient::parse(const std::string & url, std::string & scheme, std::string & target)
{
    std::string::size_type pos = url.find("://");
    if (std::string::npos != pos)
    {
        pos += 3;
    }
    else
    {
        pos = 0;
    }

    pos = url.find('/', pos);
    if (std::string::npos != pos)
    {
        scheme = url.substr(0, pos);
        target = url.substr(pos);
    }
    else
    {
        scheme = url;
        target = "/";
    }
}

void HttpClient::get(const std::string & scheme, const std::string & target, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().get(scheme, target, headers, callback);
}

void HttpClient::get(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback)
{
    Singleton<HttpManager>::instance().get(scheme, target, headers, callback);
}

void HttpClient::post(const std::string & scheme, const std::string & target, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    std::string request;
    Singleton<HttpManager>::instance().post(scheme, target, headers, request, callback);
}

void HttpClient::post(const std::string & scheme, const std::string & target, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().post(scheme, target, headers, request, callback);
}

void HttpClient::post(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback)
{
    Singleton<HttpManager>::instance().post(scheme, target, headers, request, callback);
}

void HttpClient::upload(const std::string & scheme, const std::string & target, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    std::multimap<std::string, std::string> forms;
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::download(const std::string & scheme, const std::string & target, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, [](uint64_t, uint64_t){ return true; });
}

void HttpClient::download(const std::string & scheme, const std::string & target, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, progress);
}

void HttpClient::download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback)
{
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, [](uint64_t, uint64_t){ return true; });
}

void HttpClient::download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, progress);
}

void HttpClient::get(const std::string & url, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().get(scheme, target, headers, callback);
}

void HttpClient::get(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    Singleton<HttpManager>::instance().get(scheme, target, headers, callback);
}

void HttpClient::post(const std::string & url, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    std::string request;
    Singleton<HttpManager>::instance().post(scheme, target, headers, request, callback);
}

void HttpClient::post(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    Singleton<HttpManager>::instance().post(scheme, target, headers, request, callback);
}

void HttpClient::upload(const std::string & url, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    std::multimap<std::string, std::string> forms;
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::upload(const std::string & url, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::upload(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    Singleton<HttpManager>::instance().upload(scheme, target, headers, forms, src_file_path, dst_file_name, callback);
}

void HttpClient::download(const std::string & url, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, [](uint64_t, uint64_t){ return true; });
}

void HttpClient::download(const std::string & url, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    std::multimap<std::string, std::string> headers;
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, progress);
}

void HttpClient::download(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, [](uint64_t, uint64_t){ return true; });
}

void HttpClient::download(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress)
{
    std::string scheme;
    std::string target;
    parse(url, scheme, target);
    Singleton<HttpManager>::instance().download(scheme, target, headers, save_file_path, callback, progress);
}

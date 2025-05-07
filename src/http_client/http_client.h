/********************************************************
 * Description : http client
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H


#include <cstdint>
#include <map>
#include <string>
#include <functional>

class HttpClient
{
public:
    void parse(const std::string & url, std::string & scheme, std::string & target);

public:
    void get(const std::string & scheme, const std::string & target, const std::function<void(bool result, const std::string & response)> & callback);
    void get(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & scheme, const std::string & target, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & scheme, const std::string & target, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & scheme, const std::string & target, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & scheme, const std::string & target, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & scheme, const std::string & target, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress);
    void download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress);

public:
    void get(const std::string & url, const std::function<void(bool result, const std::string & response)> & callback);
    void get(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & url, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & url, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & url, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & url, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & url, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress);
    void download(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & url, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress);
};


#endif // HTTP_CLIENT_H

/********************************************************
 * Description : http manager
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H


#include <map>
#include <string>
#include <functional>
#include "singleton.hpp"
#include "task_actuator.hpp"

class HttpManager
{
public:
    HttpManager();
    ~HttpManager();

public:
    void get(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::function<void(bool result, const std::string & response)> & callback);
    void post(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & request, const std::function<void(bool result, const std::string & response)> & callback);
    void upload(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::multimap<std::string, std::string> & forms, const std::string & src_file_path, const std::string & dst_file_name, const std::function<void(bool result, const std::string & response)> & callback);
    void download(const std::string & scheme, const std::string & target, const std::multimap<std::string, std::string> & headers, const std::string & save_file_path, const std::function<void(bool result, const std::string & response)> & callback, const std::function<bool(uint64_t, uint64_t)> & progress);

private:
    HttpManager(const HttpManager &) = delete;
    HttpManager(HttpManager &&) = delete;
    HttpManager & operator = (const HttpManager &) = delete;
    HttpManager & operator = (HttpManager &&) = delete;

private:
    friend class Singleton<HttpManager>;

private:
    TaskActuator                        m_task_actuator;
};


#endif // HTTP_MANAGER_H

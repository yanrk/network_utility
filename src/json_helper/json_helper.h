/********************************************************
 * Description : json helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#ifndef JSON_HELPER_H
#define JSON_HELPER_H


#include "json.hpp"
#include "macros.h"

using json_t = nlohmann::json;

GOOFER_CXX_API(void) string_to_json(const std::string & str, json_t & json);

GOOFER_CXX_API(void) json_to_string(const json_t & json, std::string & str);

GOOFER_CXX_API(void) get_member_map(const json_t & json, std::map<std::string, json_t> & mem_map);

template <typename T>
void get_member_vector(const json_t & json, std::vector<T> & mem_vec)
{
    json.get_to(mem_vec);
}

template <typename T>
bool parse_element(const json_t & json, const char * name, T & value)
{
    if (nullptr == name)
    {
        return false;
    }

    json_t::const_iterator iter = json.find(name);
    if (json.end() == iter)
    {
        return false;
    }

    const json_t & sub = *iter;
    if (sub.is_null())
    {
        return false;
    }

    sub.get_to(value);

    return true;
}

GOOFER_CXX_API(bool) parse_element(const json_t & json, const char * name, json_t & value);

template <typename T>
bool build_element(json_t & json, const char * name, const T & value)
{
    if (nullptr == name)
    {
        return false;
    }

    json[name] = value;

    return true;
}

GOOFER_CXX_API(bool) build_element(json_t & json, const char * name, const char * value);


#endif // JSON_HELPER_H

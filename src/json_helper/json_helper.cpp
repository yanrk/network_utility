/********************************************************
 * Description : json helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include "json_helper.h"

void string_to_json(const std::string & str, json_t & json)
{
    json = json_t::parse(str);
}

void json_to_string(const json_t & json, std::string & str)
{
    str = json.dump();
}

void get_member_map(const json_t & json, std::map<std::string, json_t> & mem_map)
{
    json.get_to(mem_map);
}

bool parse_element(const json_t & json, const char * name, json_t & value)
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

    value = sub;

    return true;
}

bool build_element(json_t & json, const char * name, const char * value)
{
    if (nullptr == name || nullptr == value)
    {
        return false;
    }

    json[name] = value;

    return true;
}

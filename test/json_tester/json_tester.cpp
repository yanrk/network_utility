/********************************************************
 * Description : sample of json helper
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstdio>
#include "json_helper.h"

struct person_t
{
    std::string                 name;
    int                         age;
    std::vector<std::string>    hobbies;
};

static bool operator == (const person_t & lhs, const person_t & rhs)
{
    if (lhs.name != rhs.name)
    {
        return false;
    }

    if (lhs.age != rhs.age)
    {
        return false;
    }

    if (lhs.hobbies != rhs.hobbies)
    {
        return false;
    }

    return true;
}

static void to_json(json_t & json, const person_t & person) // overload
{
    build_element(json, "name", person.name);
    build_element(json, "age", person.age);
    build_element(json, "hobbies", person.hobbies);
}

static void from_json(const json_t & json, person_t & person) // overload
{
    parse_element(json, "name", person.name);
    parse_element(json, "age", person.age);
    parse_element(json, "hobbies", person.hobbies);
}

struct tester_t
{
    std::string             a;
    bool                    b;
    double                  c;
    int                     d;
    std::vector<int>        v;
    std::vector<person_t>   p;
    struct
    {
        std::string         x;
        int                 y;
    }                       s;
};

static bool operator == (const tester_t & lhs, const tester_t & rhs)
{
    if (lhs.a != rhs.a)
    {
        return false;
    }

    if (lhs.b != rhs.b)
    {
        return false;
    }

    if (lhs.c > rhs.c + 1e-9 || lhs.c + 1e-9 < rhs.c)
    {
        return false;
    }

    if (lhs.d != rhs.d)
    {
        return false;
    }

    if (lhs.v != rhs.v)
    {
        return false;
    }

    if (lhs.p != rhs.p)
    {
        return false;
    }

    if (lhs.s.x != rhs.s.x)
    {
        return false;
    }

    if (lhs.s.y != rhs.s.y)
    {
        return false;
    }

    return true;
}

static void tester_to_string(const tester_t & obj, std::string & str)
{
    json_t json_sub;
    build_element(json_sub, "x", obj.s.x);
    build_element(json_sub, "y", obj.s.y);

    json_t json_obj;
    build_element(json_obj, "a", obj.a);
    build_element(json_obj, "b", obj.b);
    build_element(json_obj, "c", obj.c);
    build_element(json_obj, "d", obj.d);
    build_element(json_obj, "v", obj.v);
    build_element(json_obj, "p", obj.p);
    build_element(json_obj, "s", json_sub);

    json_to_string(json_obj, str);
}

static void string_to_tester(const std::string & str, tester_t & obj)
{
    json_t json_obj;
    string_to_json(str, json_obj);

    parse_element(json_obj, "a", obj.a);
    parse_element(json_obj, "b", obj.b);
    parse_element(json_obj, "c", obj.c);
    parse_element(json_obj, "d", obj.d);
    parse_element(json_obj, "v", obj.v);
    parse_element(json_obj, "p", obj.p);

    json_t json_sub;
    parse_element(json_obj, "s", json_sub);
    parse_element(json_sub, "x", obj.s.x);
    parse_element(json_sub, "y", obj.s.y);
}

int main(int argc, char * argv[])
{
    try
    {
        std::string str("{\"a\":\"xyz\", \"b\":123, \"c\":false, \"d\":1.23}");
        json_t json;
        string_to_json(str, json);

        std::map<std::string, json_t> mem_map;
        get_member_map(json, mem_map);

        if (mem_map.size() == 4)
        {
            printf("get member map success\n");
        }
        else
        {
            printf("get member map failure\n");
        }
    }
    catch (...)
    {
        printf("get member map exception\n");
    }

    try
    {
        std::string str("[123, 456, 789]");
        json_t json;
        string_to_json(str, json);

        std::vector<int> mem_vec;
        get_member_vector(json, mem_vec);

        if (mem_vec.size() == 3)
        {
            printf("get member vector success\n");
        }
        else
        {
            printf("get member vector failure\n");
        }
    }
    catch (...)
    {
        printf("get member vector exception\n");
    }

    {
        tester_t src;
        src.a = "aaa";
        src.b = false;
        src.c = 1.234;
        src.d = 11111;
        src.v.push_back(1);
        src.v.push_back(2);
        src.v.push_back(3);
        src.p.push_back({"Amy", 27, {"reading", "writing"}});
        src.p.push_back({"Bob", 30, {"gaming", "coding", "acting"}});
        src.p.push_back({"Ken", 32, {"dancing", "drawing"}});
        src.s.x = "xxxxxx";
        src.s.y = 22222222;

        std::string str;
        tester_to_string(src, str);

        tester_t dst;
        string_to_tester(str, dst);

        if (src == dst)
        {
            printf("json test success\n");
        }
        else
        {
            printf("json test failure\n");
        }
    }

    return 0;
}

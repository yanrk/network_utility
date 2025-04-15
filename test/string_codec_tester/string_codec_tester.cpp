/********************************************************
 * Description : sample of string codec functions
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2024
 ********************************************************/

#include <cstdio>
#include "string_codec.h"

bool test()
{
    {
        for (int i = 0; i < 10; ++i)
        {
            std::vector<unsigned char> src(i, 'a');
            std::string dst;
            if (!base64_encode(src.data(), src.size(), dst))
            {
                printf("base64_encode failure\n");
                return false;
            }
            std::vector<unsigned char> out;
            if (!base64_decode(dst.c_str(), out))
            {
                printf("base64_decode failure\n");
                return false;
            }
            if (src != out)
            {
                printf("base64 exception\n");
                return false;
            }
        }
    }

    {
        for (int i = 1; i < 10; ++i)
        {
            std::vector<unsigned char> src(i, 'z');
            std::string dst;
            if (!hex_encode(src.data(), src.size(), dst, i % 2))
            {
                printf("hex_encode failure\n");
                return false;
            }
            std::vector<unsigned char> out;
            if (!hex_decode(dst.c_str(), out))
            {
                printf("hex_decode failure\n");
                return false;
            }
            if (src != out)
            {
                printf("hex exception\n");
                return false;
            }
        }
    }

    return true;
}

int main(int argc, char * argv[])
{
    if (test())
    {
        printf("test secure success\n");
    }
    else
    {
        printf("test secure failure\n");
    }
    return 0;
}

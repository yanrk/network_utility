/********************************************************
 * Description : string codec
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#include <cstring>
#include <algorithm>
#include "string_codec.h"

static const char s_hex_lower_table[] = 
{
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'  
};

static const char s_hex_upper_table[] = 
{
    '0', '1', '2', '3', '4', '5', '6', '7', 
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'  
};

static unsigned char hex_index(const char symbol)
{
    if ('0' <= symbol && '9' >= symbol)
    {
        return symbol - '0';
    }
    else if ('a' <= symbol && 'f' >= symbol)
    {
        return symbol - 'a' + 10;
    }
    else if ('A' <= symbol && 'F' >= symbol)
    {
        return symbol - 'A' + 10;
    }
    else
    {
        return 16;
    }
}

static bool binary_to_hex(const unsigned char src, char dst[2], bool lower)
{
    if (nullptr == dst)
    {
        return false;
    }

    if (lower)
    {
        dst[0] = s_hex_lower_table[(src >> 4) & 0x0F];
        dst[1] = s_hex_lower_table[src & 0x0F];
    }
    else
    {
        dst[0] = s_hex_upper_table[(src >> 4) & 0x0F];
        dst[1] = s_hex_upper_table[src & 0x0F];
    }

    return true;
}

template<typename byte>
static bool hex_to_binary(const char src[2], byte & dst)
{
    if (nullptr == src)
    {
        return false;
    }

    unsigned char hig = hex_index(src[0]);
    unsigned char low = hex_index(src[1]);

    if (hig > 15 || low > 15)
    {
        return false;
    }

    dst = (hig << 4) | low;

    return true;
}


bool hex_encode(const unsigned char * src, size_t src_len, std::string & dst, bool lower)
{
    dst.clear();

    if (nullptr == src)
    {
        return false;
    }

    dst.resize(src_len * 2, 0x0);

    for (size_t index = 0; index < src_len; ++index)
    {
        if (!binary_to_hex(src[index], &dst[index * 2], lower))
        {
            return false;
        }
    }

    return true;
}

bool hex_encode(const void * src, size_t src_len, std::string & dst, bool lower)
{
    return hex_encode(reinterpret_cast<const unsigned char *>(src), src_len, dst, lower);
}

bool hex_encode(const std::vector<unsigned char> & src, std::string & dst, bool lower)
{
    return hex_encode(src.data(), src.size(), dst, lower);
}

bool hex_encode(const std::vector<char> & src, std::string & dst, bool lower)
{
    return hex_encode(src.data(), src.size(), dst, lower);
}

bool hex_encode(const std::string & src, std::string & dst, bool lower)
{
    return hex_encode(src.data(), src.size(), dst, lower);
}

bool hex_encode(const char * src, std::string & dst, bool lower)
{
    return nullptr != src && hex_encode(src, strlen(src), dst, lower);
}


template <typename random_access_container>
bool hex_decode(const char * src, size_t src_len, random_access_container & dst)
{
    dst.clear();

    if (nullptr == src)
    {
        return false;
    }

    if (0 != src_len % 2)
    {
        return false;
    }

    if (0 == src_len)
    {
        return true;
    }

    dst.resize(src_len / 2, 0x0);

    for (size_t index = 0; index < src_len; index += 2)
    {
        if (!hex_to_binary(&src[index], dst[index / 2]))
        {
            return false;
        }
    }

    return true;
}

bool hex_decode(const char * src, size_t src_len, std::vector<unsigned char> & dst)
{
    return hex_decode<std::vector<unsigned char>>(src, src_len, dst);
}

bool hex_decode(const char * src, size_t src_len, std::vector<char> & dst)
{
    return hex_decode<std::vector<char>>(src, src_len, dst);
}

bool hex_decode(const char * src, size_t src_len, std::string & dst)
{
    return hex_decode<std::string>(src, src_len, dst);
}

bool hex_decode(const char * src, std::vector<unsigned char> & dst)
{
    return nullptr != src && hex_decode(src, strlen(src), dst);
}

bool hex_decode(const char * src, std::vector<char> & dst)
{
    return nullptr != src && hex_decode(src, strlen(src), dst);
}

bool hex_decode(const char * src, std::string & dst)
{
    return nullptr != src && hex_decode(src, strlen(src), dst);
}

bool hex_decode(const std::string & src, std::vector<unsigned char> & dst)
{
    return hex_decode(src.data(), src.size(), dst);
}

bool hex_decode(const std::string & src, std::vector<char> & dst)
{
    return hex_decode(src.data(), src.size(), dst);
}

bool hex_decode(const std::string & src, std::string & dst)
{
    return hex_decode(src.data(), src.size(), dst);
}


static const char s_base64_table[] = 
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
    'w', 'x', 'y', 'z', '0', '1', '2', '3', 
    '4', '5', '6', '7', '8', '9', '+', '/'  
};

static unsigned char base64_index(const char symbol)
{
    if ('A' <= symbol && 'Z' >= symbol)
    {
        return symbol - 'A' + 0;
    }
    else if ('a' <= symbol && 'z' >= symbol)
    {
        return symbol - 'a' + 26;
    }
    else if ('0' <= symbol && '9' >= symbol)
    {
        return symbol - '0' + 52;
    }
    else if ('+' == symbol)
    {
        return 62;
    }
    else if ('/' == symbol)
    {
        return 63;
    }
    else if ('=' == symbol)
    {
        return 0;
    }
    else
    {
        return 64;
    }
}


bool base64_encode(const unsigned char * src, size_t src_len, std::string & dst)
{
    dst.clear();

    if (0 == src_len)
    {
        return true;
    }

    if (nullptr == src)
    {
        return false;
    }

    size_t dst_len = (src_len + 2) / 3 * 4;
    dst.resize(dst_len, 0x0);

    size_t src_idx = 0;
    size_t dst_idx = 0;
    while (src_idx < src_len)
    {
        unsigned char tmp[3] = { 0x0 };
        size_t len = std::min<size_t>(src_len - src_idx, 3);
        memcpy(tmp, src + src_idx, len);
        src_idx += 3;
        dst[dst_idx++] = s_base64_table[((tmp[0] >> 2) & 0x3F)];
        dst[dst_idx++] = s_base64_table[((tmp[0] << 4) & 0x30) | ((tmp[1] >> 4) & 0x0F)];
        dst[dst_idx++] = s_base64_table[((tmp[1] << 2) & 0x3C) | ((tmp[2] >> 6) & 0x03)];
        dst[dst_idx++] = s_base64_table[((tmp[2]) & 0x3F)];
    }

    switch (src_len % 3)
    {
        case 1:
        {
            dst[--dst_idx] = '=';
        }
        case 2:
        {
            dst[--dst_idx] = '=';
        }
    }

    return true;
}

bool base64_encode(const void * src, size_t src_len, std::string & dst)
{
    return base64_encode(reinterpret_cast<const unsigned char *>(src), src_len, dst);
}

bool base64_encode(const std::vector<unsigned char> & src, std::string & dst)
{
    return base64_encode(src.data(), src.size(), dst);
}

bool base64_encode(const std::vector<char> & src, std::string & dst)
{
    return base64_encode(src.data(), src.size(), dst);
}

bool base64_encode(const std::string & src, std::string & dst)
{
    return base64_encode(src.data(), src.size(), dst);
}

bool base64_encode(const char * src, std::string & dst)
{
    return nullptr != src && base64_encode(src, strlen(src), dst);
}


template <typename random_access_container>
bool base64_decode(const char * src, size_t src_len, random_access_container & dst)
{
    dst.clear();

    if (nullptr == src)
    {
        return false;
    }

    if (0 != src_len % 4)
    {
        return false;
    }

    if (0 == src_len)
    {
        return true;
    }

    if (nullptr != memchr(src, '=', src_len - 2))
    {
        return false;
    }

    if (base64_index(src[src_len - 1]) > 63 || base64_index(src[src_len - 2]) > 63)
    {
        return false;
    }

    size_t pad_len = 0;
    if ('=' == src[src_len - 1])
    {
        if ('=' == src[src_len - 2])
        {
            pad_len = 2;
        }
        else
        {
            pad_len = 1;
        }
    }
    else if ('=' == src[src_len - 2])
    {
        return false;
    }

    size_t dst_len = (src_len + 3) / 4 * 3;
    dst.resize(dst_len, 0x0);

    size_t src_idx = 0;
    size_t dst_idx = 0;
    while (src_idx < src_len)
    {
        unsigned char tmp[4] = { 0x0 };
        tmp[0] = base64_index(src[src_idx++]);
        tmp[1] = base64_index(src[src_idx++]);
        tmp[2] = base64_index(src[src_idx++]);
        tmp[3] = base64_index(src[src_idx++]);
        dst[dst_idx++] = (tmp[0] << 2) | (tmp[1] >> 4);
        dst[dst_idx++] = (tmp[1] << 4) | (tmp[2] >> 2);
        dst[dst_idx++] = (tmp[2] << 6) | (tmp[3]);
    }

    dst.resize(dst_len - pad_len);

    return true;
}

bool base64_decode(const char * src, size_t src_len, std::vector<unsigned char> & dst)
{
    return base64_decode<std::vector<unsigned char>>(src, src_len, dst);
}

bool base64_decode(const char * src, size_t src_len, std::vector<char> & dst)
{
    return base64_decode<std::vector<char>>(src, src_len, dst);
}

bool base64_decode(const char * src, size_t src_len, std::string & dst)
{
    return base64_decode<std::string>(src, src_len, dst);
}

bool base64_decode(const char * src, std::vector<unsigned char> & dst)
{
    return nullptr != src && base64_decode(src, strlen(src), dst);
}

bool base64_decode(const char * src, std::vector<char> & dst)
{
    return nullptr != src && base64_decode(src, strlen(src), dst);
}

bool base64_decode(const char * src, std::string & dst)
{
    return nullptr != src && base64_decode(src, strlen(src), dst);
}

bool base64_decode(const std::string & src, std::vector<unsigned char> & dst)
{
    return base64_decode(src.data(), src.size(), dst);
}

bool base64_decode(const std::string & src, std::vector<char> & dst)
{
    return base64_decode(src.data(), src.size(), dst);
}

bool base64_decode(const std::string & src, std::string & dst)
{
    return base64_decode(src.data(), src.size(), dst);
}

/********************************************************
 * Description : string codec
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Version     : 1.0
 * Copyright(C): 2025
 ********************************************************/

#ifndef STRING_CODEC_H
#define STRING_CODEC_H


#include <string>
#include <vector>

bool hex_encode(const unsigned char * src, size_t src_len, std::string & dst, bool lower);
bool hex_encode(const void * src, size_t src_len, std::string & dst, bool lower);
bool hex_encode(const std::vector<unsigned char> & src, std::string & dst, bool lower);
bool hex_encode(const std::vector<char> & src, std::string & dst, bool lower);
bool hex_encode(const std::string & src, std::string & dst, bool lower);
bool hex_encode(const char * src, std::string & dst, bool lower);

bool hex_decode(const char * src, size_t src_len, std::vector<unsigned char> & dst);
bool hex_decode(const char * src, size_t src_len, std::vector<char> & dst);
bool hex_decode(const char * src, size_t src_len, std::string & dst);
bool hex_decode(const char * src, std::vector<unsigned char> & dst);
bool hex_decode(const char * src, std::vector<char> & dst);
bool hex_decode(const char * src, std::string & dst);
bool hex_decode(const std::string & src, std::vector<unsigned char> & dst);
bool hex_decode(const std::string & src, std::vector<char> & dst);
bool hex_decode(const std::string & src, std::string & dst);

bool base64_encode(const unsigned char * src, size_t src_len, std::string & dst);
bool base64_encode(const void * src, size_t src_len, std::string & dst);
bool base64_encode(const std::vector<unsigned char> & src, std::string & dst);
bool base64_encode(const std::vector<char> & src, std::string & dst);
bool base64_encode(const std::string & src, std::string & dst);
bool base64_encode(const char * src, std::string & dst);

bool base64_decode(const char * src, size_t src_len, std::vector<unsigned char> & dst);
bool base64_decode(const char * src, size_t src_len, std::vector<char> & dst);
bool base64_decode(const char * src, size_t src_len, std::string & dst);
bool base64_decode(const char * src, std::vector<unsigned char> & dst);
bool base64_decode(const char * src, std::vector<char> & dst);
bool base64_decode(const char * src, std::string & dst);
bool base64_decode(const std::string & src, std::vector<unsigned char> & dst);
bool base64_decode(const std::string & src, std::vector<char> & dst);
bool base64_decode(const std::string & src, std::string & dst);


#endif // STRING_CODEC_H

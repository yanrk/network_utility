/********************************************************
 * Description : macros of goofer
 * Author      : yanrk
 * Email       : yanrkchina@163.com
 * Blog        : blog.csdn.net/cxxmaker
 * Version     : 2.0
 * History     : goofer library
 * Copyright(C): 2025
 ********************************************************/

#ifndef GOOFER_MACROS_H
#define GOOFER_MACROS_H


#if defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)
    #define GOOFER_OS_IS_WIN
    #undef  GOOFER_OS_IS_MAC
    #undef  GOOFER_OS_IS_LINUX
#elif (defined(__APPLE__) && defined(__MACH__)) || defined(__MACOSX__) || defined(macintosh)
    #undef  GOOFER_OS_IS_WIN
    #define GOOFER_OS_IS_MAC
    #undef  GOOFER_OS_IS_LINUX
#else
    #undef  GOOFER_OS_IS_WIN
    #undef  GOOFER_OS_IS_MAC
    #define GOOFER_OS_IS_LINUX
#endif // defined(_MSC_VER) || defined(_WIN32) || defined(_WIN64)

#if defined(_WIN64) || defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)
    #define GOOFER_OS_IS_64_BIT 1
    #define GOOFER_OS_IS_32_BIT 0
#elif defined(_WIN32) || defined(_M_IX86) || defined(__i386) || defined(__arm__)
    #define GOOFER_OS_IS_64_BIT 0
    #define GOOFER_OS_IS_32_BIT 1
#else
    #if UINTPTR_MAX == UINT64_MAX
        #define GOOFER_OS_IS_64_BIT 1
        #define GOOFER_OS_IS_32_BIT 0
    #elif UINTPTR_MAX == UINT32_MAX
        #define GOOFER_OS_IS_64_BIT 0
        #define GOOFER_OS_IS_32_BIT 1
    #else
        #define GOOFER_OS_IS_64_BIT 0
        #define GOOFER_OS_IS_32_BIT 0
    #endif
#endif // defined(_WIN64) || defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__)

#ifdef GOOFER_OS_IS_WIN
    #define GOOFER_CDECL           __cdecl
    #define GOOFER_STDCALL         __stdcall
    #ifdef EXPORT_GOOFER_DLL
        #define GOOFER_API         __declspec(dllexport)
    #else
        #ifdef USE_GOOFER_DLL
            #define GOOFER_API     __declspec(dllimport)
        #else
            #define GOOFER_API
        #endif // USE_GOOFER_DLL
    #endif // EXPORT_GOOFER_DLL
#else
    #define GOOFER_CDECL
    #define GOOFER_STDCALL
    #define GOOFER_API
#endif // GOOFER_OS_IS_WIN

#define GOOFER_CXX_API(return_type)             extern GOOFER_API return_type GOOFER_CDECL

#ifdef __cplusplus
    #define GOOFER_C_API(return_type)           extern "C" GOOFER_API return_type GOOFER_CDECL
    #define GOOFER_EXTERN_TYPE(variable_type)   extern "C" GOOFER_API variable_type
    #define GOOFER_EXTERN_C_BEGIN               extern "C" {
    #define GOOFER_EXTERN_C_END                 }
    #define GOOFER_NAMESPACE_BEGIN(name)        namespace name {
    #define GOOFER_NAMESPACE_END                }
    #define GOOFER_USING_NAMESPACE(name)        using namespace name;
    #define GOOFER_INLINE                       inline
    #define GOOFER_DEFAULT_ARG(var)             = var
#else
    #define GOOFER_C_API(return_type)           extern GOOFER_API return_type GOOFER_CDECL
    #define GOOFER_EXTERN_TYPE(variable_type)   extern GOOFER_API variable_type
    #define GOOFER_EXTERN_C_BEGIN
    #define GOOFER_EXTERN_C_END
    #define GOOFER_NAMESPACE_BEGIN(name)
    #define GOOFER_NAMESPACE_END
    #define GOOFER_USING_NAMESPACE(name)
    #define GOOFER_INLINE                       static
    #define GOOFER_DEFAULT_ARG(var)
#endif // __cplusplus

#define LAMBDA_REF(...) ((void)((void)__VA_ARGS__))
#define PARAMS_IGN(...) ((void)((void)__VA_ARGS__))

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
    #define GOOFER_I64_VAL(n)   n##I64
    #define GOOFER_U64_VAL(n)   n##UI64
    #define GOOFER_U64_FMT      "%I64u"
    #define GOOFER_I64_FMT      "%I64d"
    #define GOOFER_I64_ARG(x)   ((__int64)(x))
    #define GOOFER_U64_ARG(x)   ((unsigned __int64)(x))
#else
    #define GOOFER_I64_VAL(n)   n##LL
    #define GOOFER_U64_VAL(n)   n##ULL
    #define GOOFER_U64_FMT      "%llu"
    #define GOOFER_I64_FMT      "%lld"
    #define GOOFER_I64_ARG(x)   ((long long)(x))
    #define GOOFER_U64_ARG(x)   ((unsigned long long)(x))
#endif // defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)

#ifdef GOOFER_OS_IS_WIN
    #define GOOFER_LIBRARY_PREFIX   ""
    #define GOOFER_LIBRARY_SUFFIX   ".dll"
#elif defined(GOOFER_OS_IS_MAC)
    #define GOOFER_LIBRARY_PREFIX   "lib"
    #define GOOFER_LIBRARY_SUFFIX   ".dylib"
#else
    #define GOOFER_LIBRARY_PREFIX   "lib"
    #define GOOFER_LIBRARY_SUFFIX   ".so"
#endif // GOOFER_OS_IS_WIN

#define GOOFER_LIBRARY_NAME(name)   GOOFER_LIBRARY_PREFIX name GOOFER_LIBRARY_SUFFIX

#ifdef GOOFER_OS_IS_WIN
    #define goofer_system_error()                           (::GetLastError())
    #define goofer_network_error()                          (::WSAGetLastError())
    #define goofer_library_function(handle, type, name)     type name = (type)(GetProcAddress(handle, #name))
#else
    #define goofer_system_error()                           (errno + 0)
    #define goofer_network_error()                          (errno + 0)
    #define goofer_library_function(handle, type, name)     type name = (type)(dlsym(handle, #name))
#endif // GOOFER_OS_IS_WIN

#ifndef __FILENAME__
    #ifdef GOOFER_OS_IS_WIN
        #define __FILENAME__    (strrchr("\\" __FILE__, '\\') + 1)
    #else
        #define __FILENAME__    (strrchr("/" __FILE__, '/') + 1)
    #endif // GOOFER_OS_IS_WIN
#endif // __FILENAME__


#endif // GOOFER_MACROS_H

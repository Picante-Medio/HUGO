#pragma once

#ifdef _WIN32
    #ifdef BUILD_UTILS_API
        #define UTILS_API __declspec(dllexport)
    #else
        #define UTILS_API __declspec(dllimport)
    #endif
#else
    #ifdef BUILD_UTILS_API
        #define UTILS_API __attribute__((visibility("default")))
    #else
        #define UTILS_API
    #endif
#endif

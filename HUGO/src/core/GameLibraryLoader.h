#pragma once

#include <string>
#include <memory>

#include "HugoError.h"
#include "IScene.h"
#include "HugoParsingVariant.h"
#include "System.h"
#include "LuaBinder.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define LIBRARY_HANDLE HMODULE
    #define LOAD_DYNAMIC_LIBRARY(path) LoadLibraryA(path.c_str())
    #define GET_FUNC(handle, func) GetProcAddress(handle, func)
    #define CLOSE_DYNAMIC_LIBRARY(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    #define LIBRARY_HANDLE void*
    #define LOAD_DYNAMIC_LIBRARY(path) dlopen(path.c_str(), RTLD_NOW)
    #define GET_FUNC(handle, func) dlsym(handle, func)
    #define CLOSE_DYNAMIC_LIBRARY(handle) dlclose(handle)
#endif

namespace HUGO {

class GameLibraryLoader {

    GameLibraryLoader(const std::string& gameLibraryName);

    static inline std::unique_ptr<GameLibraryLoader> loaderInstance = nullptr;
    LIBRARY_HANDLE gameHandle = nullptr;

public:

    static void init(const std::string& gameLibraryName);
    static void destroy();

    static GameLibraryLoader& instance();

    ~GameLibraryLoader();
};

}

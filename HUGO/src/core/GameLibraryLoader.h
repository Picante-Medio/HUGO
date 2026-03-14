#pragma once

#include <string>
#include <memory>

#include "IScene.h"
#include "HugoParsingVariant.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define LIBRARY_HANDLE HMODULE
    #define LOAD_DYNAMIC_LIBRARY(path) LoadLibraryA(path.c_str())
    #define GET_FUNC(handle, func) GetProcAddress(handle, func)
    #define CLOSE_DYNAMIC_LIBRARY(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    #define LIBRARY_HANDLE void*
    #define LOAD_DYNAMIC_LIBRARY(path) dlopen(path.c_str(), RTLD_LAZY)
    #define GET_FUNC(handle, func) dlsym(handle, func)
    #define CLOSE_DYNAMIC_LIBRARY(handle) dlclose(handle)
#endif

typedef void (*GenerateComponentFn)(std::shared_ptr<IScene>, std::string, std::unordered_map<std::string, HVariant>);

namespace HUGO {

class GameLibraryLoader {

    GameLibraryLoader(std::string gameLibraryName);

    static inline std::shared_ptr<GameLibraryLoader> instance = nullptr;

    LIBRARY_HANDLE gameHandle = nullptr;
    GenerateComponentFn generateComponentPointer = nullptr;
    
public:

    ~GameLibraryLoader();

    static void init(std::string gameLibraryName);
    static void destroy();

    void GenerateComponent(std::shared_ptr<IScene>, std::string, std::unordered_map<std::string, HVariant>);



};

}
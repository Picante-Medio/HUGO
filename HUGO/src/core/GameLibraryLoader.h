#pragma once

#include <string>
#include <memory>

#include "HugoError.h"
#include "IScene.h"
#include "HugoParsingVariant.h"
#include "System.h"

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

namespace HUGO {

class GameLibraryLoader {

    typedef void (*InitializeGameFn)(std::shared_ptr<HUGO::ComponentRegistry>);

    typedef bool (*GenerateComponentFn)(std::shared_ptr<HUGO::IScene>, EntityID, const std::string&,
        const std::unordered_map<std::string, HVariant>&);

    typedef bool (*GenerateAndGetSystemFn)(std::shared_ptr<HUGO::IScene>, const std::string&,
        std::shared_ptr<HUGO::System>&);


    GameLibraryLoader(const std::string& gameLibraryName);

    static inline std::unique_ptr<GameLibraryLoader> loaderInstance = nullptr;

    LIBRARY_HANDLE gameHandle = nullptr;
    InitializeGameFn initializeGameFnPointer = nullptr;
    GenerateComponentFn generateComponentFnPointer = nullptr;    
    GenerateAndGetSystemFn generateAndGetSystemFnPointer = nullptr;

public:

    static void init(const std::string& gameLibraryName);
    static void destroy();

    static GameLibraryLoader& instance();

    ~GameLibraryLoader();

    ErrorOrVoid GenerateComponent(std::shared_ptr<HUGO::IScene> scene, EntityID ent, const std::string& component, 
        const std::unordered_map<std::string, HVariant>& componentMembers);

    ErrorOrType<std::shared_ptr<HUGO::System>>GenerateAndGetSystem(std::shared_ptr<HUGO::IScene> scene, 
        const std::string& systemName);
};

}
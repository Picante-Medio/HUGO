#include "GameLibrary.h"

#include "LuaBinder.h"

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #define LIBRARY_HANDLE HMODULE
    #define LOAD_DYNAMIC_LIBRARY(path) LoadLibraryA(path.c_str())
    #define CLOSE_DYNAMIC_LIBRARY(handle) FreeLibrary(handle)
#else
    #include <dlfcn.h>
    #define LIBRARY_HANDLE void*
    #define LOAD_DYNAMIC_LIBRARY(path) dlopen(path.c_str(), RTLD_NOW)
    #define CLOSE_DYNAMIC_LIBRARY(handle) dlclose(handle)
#endif

struct HUGO::GameLibrary::Impl{
    LIBRARY_HANDLE gameHandle = nullptr;
};

HUGO::GameLibrary::GameLibrary(const std::string& gameLibraryName): pImpl(new Impl{}) {

    LIBRARY_HANDLE& gameHandle = pImpl->gameHandle;

    #ifdef NDEBUG
        std::string debugPostfix = "";
    #else
        std::string debugPostfix = "d"; // this should be a macro or smthing like that
    #endif

    #ifdef _WIN32
        std::string prefix = "";
        std::string extension = ".dll";
    #else
        std::string prefix = "lib";
        std::string extension = ".so";
    #endif

    std::string completeGameLibraryName = prefix + gameLibraryName + debugPostfix + extension;

    gameHandle = LOAD_DYNAMIC_LIBRARY(completeGameLibraryName);

    if (!gameHandle) {
        throw std::runtime_error("[GameLibraryLoader] Error loading " + gameLibraryName);
    }
}

HUGO::GameLibrary::GameLibrary(): pImpl(nullptr) {}

HUGO::GameLibrary::~GameLibrary() {

    if (!pImpl) return; 

    LIBRARY_HANDLE& gameHandle = pImpl->gameHandle;

    CLOSE_DYNAMIC_LIBRARY(gameHandle);
    gameHandle = nullptr;
}
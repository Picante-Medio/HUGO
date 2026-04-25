#include "GameLibraryLoader.h"

#include "LuaBinder.h"

HUGO::GameLibraryLoader::GameLibraryLoader(const std::string& gameLibraryName) {

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

HUGO::GameLibraryLoader::~GameLibraryLoader() {
    CLOSE_DYNAMIC_LIBRARY(gameHandle);
    gameHandle = nullptr;
}

void
HUGO::GameLibraryLoader::init(const std::string& gameLibraryName) {

    if (loaderInstance == nullptr) {
        loaderInstance = std::unique_ptr<GameLibraryLoader>(new GameLibraryLoader(gameLibraryName));
    }
#ifndef NDEBUG
    else {
        std::cerr << "[GameLibraryLoader] instancia ya inicializada";
    }
#endif
}

HUGO::GameLibraryLoader&
HUGO::GameLibraryLoader::instance() {
    if (loaderInstance == nullptr) {
        throw std::runtime_error("[GameLibraryLoader] no library loaded");
    }
    return *loaderInstance;
}

void 
HUGO::GameLibraryLoader::destroy() {
    if (loaderInstance != nullptr) {
        loaderInstance = nullptr;
    }
}
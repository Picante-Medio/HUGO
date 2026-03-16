#include "GameLibraryLoader.h"

HUGO::GameLibraryLoader::GameLibraryLoader(const std::string& gameLibraryName) {

    gameHandle = LOAD_DYNAMIC_LIBRARY(gameLibraryName);
    if (!gameHandle) {
        throw std::runtime_error("[GameLibraryLoader] Error loading " + gameLibraryName);
    }
    generateComponentFnPointer = reinterpret_cast<GenerateComponentFn>(GET_FUNC(gameHandle, "GenerateComponent"));
    if (!generateComponentFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] void get_game(std::shared_ptr<IScene>, " 
            "std::string, std::unordered_map<std::string, HVariant>) not found in " + gameLibraryName);
    }
}

HUGO::GameLibraryLoader::~GameLibraryLoader() {
    CLOSE_DYNAMIC_LIBRARY(gameHandle);
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

ErrorOrVoid 
HUGO::GameLibraryLoader::GenerateComponent(std::shared_ptr<IScene> scene, const std::string& componentName, 
    const std::unordered_map<std::string, HVariant>& componentMembers) {

    if (generateComponentFnPointer(scene, componentName, componentMembers)) {
        return ErrorOrVoid();
    }
    
    return ErrorOrVoid(StringError{"No se ha podido generar el componente de tipo: " + componentName});
}
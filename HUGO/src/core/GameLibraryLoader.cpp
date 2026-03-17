#include "GameLibraryLoader.h"

#include "ComponentRegistryInitializer.h"

HUGO::GameLibraryLoader::GameLibraryLoader(const std::string& gameLibraryName) {

    gameHandle = LOAD_DYNAMIC_LIBRARY(gameLibraryName);
    if (!gameHandle) {
        throw std::runtime_error("[GameLibraryLoader] Error loading " + gameLibraryName);
    }
    initializeGameFnPointer = reinterpret_cast<InitializeGameFn>(GET_FUNC(gameHandle, "InitializeGame"));
    if (!initializeGameFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] void InitializeGame(std::shared_ptr<ComponentRegistry> " 
            "not found in " + gameLibraryName);
    }
    generateComponentFnPointer = reinterpret_cast<GenerateComponentFn>(GET_FUNC(gameHandle, "GenerateComponent"));
    if (!generateComponentFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] bool GenerateComponent(std::shared_ptr<IScene>, EntityID, " 
            "const std::string&, const std::unordered_map<std::string, HVariant>&) not found in " + gameLibraryName);
    }

    initializeGameFnPointer(ComponentRegistryInitializer::getInstance());
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
HUGO::GameLibraryLoader::GenerateComponent(std::shared_ptr<IScene> scene, EntityID ent, const std::string& componentName, 
    const std::unordered_map<std::string, HVariant>& componentMembers) {

    if (generateComponentFnPointer(scene, ent, componentName, componentMembers)) {
        return ErrorOrVoid();
    }
    
    return ErrorOrVoid(StringError{"No se ha podido generar el componente de tipo: " + componentName});
}
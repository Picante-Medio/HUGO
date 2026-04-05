#include "GameLibraryLoader.h"

#include "ComponentRegistryInitializer.h"
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
    initializeGameFnPointer = reinterpret_cast<InitializeGameFn>(GET_FUNC(gameHandle, "InitializeGame"));
    if (!initializeGameFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] void InitializeGame(std::shared_ptr<ComponentRegistry> " 
            "not found in " + gameLibraryName);
    }
    generateComponentFnPointer = reinterpret_cast<GenerateComponentFn>(GET_FUNC(gameHandle, "GenerateComponent"));
    if (!generateComponentFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] bool GenerateComponent(std::shared_ptr<HUGO::IScene>, EntityID, " 
            "const std::string&, const std::unordered_map<std::string, HVariant>&) not found in " + gameLibraryName);
    }
    generateAndGetSystemFnPointer = reinterpret_cast<GenerateAndGetSystemFn>(GET_FUNC(gameHandle, "GenerateAndGetSystem"));
    if (!generateAndGetSystemFnPointer) {
        throw std::runtime_error("[GameLibraryLoader] bool GenerateAndGetSystemFn(std::shared_ptr<HUGO::IScene>, "
            "EntityID, const std::string&, std::shared_ptr<System>&)  not found in " + gameLibraryName);
    }

    initializeGameFnPointer(ComponentRegistryInitializer::getInstance(), LuaBinder::getInstance());
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
HUGO::GameLibraryLoader::GenerateComponent(std::shared_ptr<HUGO::IScene> scene, EntityID ent, const std::string& componentName, 
    const std::unordered_map<std::string, HVariant>& componentMembers) {

    if (generateComponentFnPointer(scene, ent, componentName, componentMembers)) {
        return ErrorOrVoid();
    }
    
    return ErrorOrVoid(StringError{"No se ha podido generar el componente de tipo: " + componentName});
}

ErrorOrType<std::shared_ptr<HUGO::System>>
HUGO::GameLibraryLoader::GenerateAndGetSystem(std::shared_ptr<HUGO::IScene> scene, const std::string& systemName) {

    std::shared_ptr<System> returnedSystem = nullptr;

    if (generateAndGetSystemFnPointer(scene, systemName, returnedSystem)) {
        return ErrorOrType(returnedSystem);
    }
    
    return ErrorOrType<std::shared_ptr<System>>(
        StringError{"No se ha podido generar el sistema de tipo: " + systemName});
}

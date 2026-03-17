#include "GameEntryPoint.h"

#include "TestComponent.h"

#include <iostream>

void InitializeGame(std::shared_ptr<ComponentRegistry> registry) {
    ComponentRegistry::setInstance(registry);
}

bool GenerateComponent(std::shared_ptr<IScene> scene, EntityID ent, const std::string& component, 
    const std::unordered_map<std::string, HVariant>& componentMembers) {

    ErrorOrVoid makeComponentResult = HUGO::ComponentFactory::makeComponent(scene, ent, component, componentMembers);

    if (makeComponentResult.isError()) {

        #ifndef NDEBUG
        std::cerr << makeComponentResult.getError().description() << "\n";
        #endif

        return false;
    }

    return true;
}

bool GenerateAndGetSystem(std::shared_ptr<IScene> scene, const std::string& systemName) {
    return false;
}
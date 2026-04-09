#include "GameEntryPoint.h"

#include "LuaBinder.h"
#include "TestComponent.h"
#include "TestSystem.h"

#include <iostream>

bool GenerateComponent(std::shared_ptr<HUGO::IScene> scene, EntityID ent, const std::string& component, 
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

bool GenerateAndGetSystem(std::shared_ptr<HUGO::IScene> scene, const std::string& systemName, 
    std::shared_ptr<HUGO::System>& returnSystem) {

    ErrorOrType<std::shared_ptr<HUGO::System>> makeSystemResult = HUGO::SystemFactory::makeSystem(scene, systemName);

    if (makeSystemResult.isError()) {
        #ifndef NDEBUG
        std::cerr << makeSystemResult.getError().description() << "\n";
        #endif

        return false;
    }

    returnSystem = makeSystemResult.getValue();

    return true;
}

#include "GameEntryPoint.h"

#include <iostream>

bool GenerateComponent(IScene& scene, EntityID ent, const std::string& component, 
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
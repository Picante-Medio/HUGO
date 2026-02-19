#include "../../ecs/Component.h"

#include <string>
struct CreateOgreObjectComponent: public Component
{
    std::string meshName;
    std::string materialName;
    
    CreateOgreObjectComponent(EntityID entity, const std::string& meshName, const std::string& materialName = "") 
        :Component(entity), meshName(meshName), materialName(materialName) {}
};
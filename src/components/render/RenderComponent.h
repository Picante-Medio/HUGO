#include "../../ecs/Component.h"

#include <string>
struct RenderComponent: public Component
{
    std::string meshName;
    std::string materialName;
    RenderComponent(EntityID entity, const std::string& meshName = " ", const std::string& materialName = " ") 
        :Component(entity), meshName(meshName), materialName(materialName) {}
};
#pragma once

#include "Component.h"
#include <string>
#include "HugoTypes.h"

struct RenderComponent: public Component
{
    std::string meshName;
    std::string materialName;
    RenderComponent(EntityID entity, 
                        const std::string& meshName,
                        const std::string& materialName="") 
        : Component(entity), meshName(meshName), materialName(materialName){};
    
    friend class RenderSystem;
private:
    HUGO::OgreObjectID objectID = HUGO::INVALID_OGRE_OBJECT_ID;
};

struct MaterialDirtyTag : public Component {
    std::string materialName;
    MaterialDirtyTag(EntityID entity, const std::string& materialName) 
        : Component(entity), materialName(materialName) {}
};

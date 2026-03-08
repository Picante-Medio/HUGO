#pragma once
#include "Component.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(RenderComponent)

private:
    HUGO::OgreObjectID objectID = HUGO::INVALID_OGRE_OBJECT_ID;
    HUGO::String meshName;
    HUGO::String materialName;
public:
    RenderComponent(EntityID entity, 
                        const HUGO::String& meshName,
                        const HUGO::String& materialName="") 
        : Component(entity), meshName(meshName), materialName(materialName){};
    
    void setMeshName(const HUGO::String& newMeshName);
    void setMaterialName(const HUGO::String& newMaterialName);
    HUGO::String getMeshName() const;
    HUGO::String getMaterialName() const;
    
    friend class RenderSystem;

END_HCOMPONENT()


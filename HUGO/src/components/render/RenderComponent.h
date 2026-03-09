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
    
    // Setters
    void setMeshName(const HUGO::String& newMeshName) { meshName = newMeshName; }
    void setMaterialName(const HUGO::String& newMaterialName) { materialName = newMaterialName; }
    // Getters
    HUGO::String getMeshName() const { return meshName; }
    HUGO::String getMaterialName() const { return materialName; }
    
    friend class RenderSystem;

END_HCOMPONENT()


#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(RenderComponent)

private:
    HUGO::OgreObjectID objectID = HUGO::INVALID_OGRE_OBJECT_ID;
    HUGO::String meshName="cube.mesh";
    HUGO::String materialName="";
public:
    // Setter
    void setMeshName(const HUGO::String& newMeshName) { meshName = newMeshName; }
    void setMaterialName(const HUGO::String& newMaterialName) { materialName = newMaterialName; }
    // Getters
    HUGO::String getMeshName() const { return meshName; }
    HUGO::String getMaterialName() const { return materialName; }
    
    friend class RenderSystem;

END_HCOMPONENT()


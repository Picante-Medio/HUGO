#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"
#include "OgreModule.h"

HCOMPONENT(RenderComponent)

private:
    HUGO::OgreObjectID objectID = HUGO::INVALID_OGRE_OBJECT_ID;
    HUGO::String meshName="cube.mesh";
    HUGO::String materialName="";
public:
    // Setter
    inline void setMeshName(const HUGO::String& newMeshName) { meshName = newMeshName; }
    inline void setMaterialName(const HUGO::String& newMaterialName) { materialName = newMaterialName; }
    // Getters
    inline HUGO::String getMeshName() const { return meshName; }
    inline HUGO::String getMaterialName() const { return materialName; }
    
    friend class RenderSystem;

END_HCOMPONENT()


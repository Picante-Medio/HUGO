#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(RenderComponent)
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, meshName, "cube.mesh")
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, materialName, "")

    // Setter
    inline void setMeshName(const HUGO::String& newMeshName) { meshName = newMeshName; }
    inline void setMaterialName(const HUGO::String& newMaterialName) { materialName = newMaterialName; }
    // Getters
    inline HUGO::String getMeshName() const { return meshName; }
    inline HUGO::String getMaterialName() const { return materialName; }
    
END_HCOMPONENT()


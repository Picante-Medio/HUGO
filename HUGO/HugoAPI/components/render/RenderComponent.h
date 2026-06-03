#pragma once
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(RenderComponent)
    DEFAULT_SERIALIZED_MEMBER(std::string, meshName, "cube.mesh")
    DEFAULT_SERIALIZED_MEMBER(std::string, materialName, "")
    DEFAULT_SERIALIZED_MEMBER(bool, isVisible, true)

    // Setter
    inline void setMeshName(const std::string& newMeshName) { meshName = newMeshName; }
    inline void setMaterialName(const std::string& newMaterialName) { materialName = newMaterialName; }
    // Getters
    inline std::string getMeshName() const { return meshName; }
    inline std::string getMaterialName() const { return materialName; }
    
END_HCOMPONENT()


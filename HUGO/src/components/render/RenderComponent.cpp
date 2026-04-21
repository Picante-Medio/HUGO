#pragma once
#include "RenderComponent.h"

void RenderComponent::setMeshName(const HUGO::String &newMeshName)
{
    meshName = newMeshName;
}

void RenderComponent::setMaterialName(const HUGO::String &newMaterialName)
{
    materialName = newMaterialName;
}

HUGO::String RenderComponent::getMeshName() const
{
    return meshName;
}

HUGO::String RenderComponent::getMaterialName() const
{
    return materialName;
}

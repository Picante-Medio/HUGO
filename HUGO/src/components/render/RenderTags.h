#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

struct CameraDirtyTag : public Component {
    CameraDirtyTag(EntityID entity) : Component(entity) {}
};

struct ViewportDirtyTag : public Component {
    ViewportDirtyTag(EntityID entity) : Component(entity) {}
};

struct LightDirtyTag : public Component {
    LightDirtyTag(EntityID entity) : Component(entity) {}
};

struct MaterialDirtyTag : public Component {
    HUGO::String materialName;
    MaterialDirtyTag(EntityID entity, const HUGO::String& materialName) 
        : Component(entity), materialName(materialName) {}
};

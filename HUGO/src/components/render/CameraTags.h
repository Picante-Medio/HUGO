#pragma once

#include "Component.h"
#include "HugoMath.h"

struct CameraDirtyTag : public Component {
    CameraDirtyTag(EntityID entity) : Component(entity) {}
};

struct ViewportDirtyTag : public Component {
    ViewportDirtyTag(EntityID entity) : Component(entity) {}
};

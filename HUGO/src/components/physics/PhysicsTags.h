#pragma once

#include "Component.h"

struct StaticRBTag : public Component {
    StaticRBTag(EntityID entity) : Component(entity) {}
};

struct KinematicRBTag : public Component {
    KinematicRBTag(EntityID entity) : Component(entity) {}
};

struct DynamicRBTag : public Component {
    DynamicRBTag(EntityID entity) : Component(entity) {}
};
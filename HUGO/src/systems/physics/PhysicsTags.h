#pragma once

#include "Component.h"

struct StaticRBTag : public Component {
    StaticRBTag(EntityID entity) : Component(entity) {}
};

struct KinematicRBTag : public Component {
    KinematicRBTag(EntityID entity) : Component(entity) {}
};

//! This isn't neccessary, any new rb will have an invalid ID
struct NewRBTag : public Component {
    NewRBTag(EntityID entity) : Component(entity) {}
};
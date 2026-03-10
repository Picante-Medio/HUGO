#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include "Shape.h"
#include <variant>

struct BoxColliderInfo {
    HUGO::Vector3 halfExtents;
};

struct SphereColliderInfo {
    float r;
};

struct CapsuleColliderInfo {
    float r;
    float height;
};

struct CylinderColliderInfo {
    HUGO::Vector3 halfExtents;
};

struct ConeColliderInfo {
    float r;
    float height;
};

HCOMPONENT(ColliderComponent)

END_HCOMPONENT()
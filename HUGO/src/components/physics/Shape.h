#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
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

struct Shape{

    friend class BulletSimulation;

    // Default constructor for macros
    Shape(){
        BoxColliderInfo mInfo;
        mInfo.halfExtents = HUGO::Vector3(.5f, .5f, .5f);
        info = mInfo;
    }

    // Shape's geometry is determined by the variant. Which struct is held can be checked with std::holds_alternative
    std::variant<BoxColliderInfo, SphereColliderInfo, CapsuleColliderInfo, CylinderColliderInfo, ConeColliderInfo> info;

private:
    HUGO::CollisionShapeID id = HUGO::INVALID_SHAPE_ID;
};

#pragma once

#include "HugoMath.h"
#include "HugoTypes.h"
#include <variant>

#include "HugoComponentMacros.h"

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

    // ----------------------Attributes----------------------
    enum UpdateFlags{
        RESTITUTION_UPDATED = 1,
        FRICTION_UPDATED = 2,
        TRANSFORM_OFFSET_UPDATED = 4,
        ACTIVE_UPDATED = 8
    };

    int flags = 0;

    float restitution = 0.f;
    float friction = 0.5f;

    HUGO::Vector3 positionOffset = HUGO::Vector3();
    HUGO::Quaternion rotationOffset = HUGO::Quaternion();

    bool active = true;

    std::vector<std::reference_wrapper<ColliderComponent>> collisionInfo;
    std::vector<std::reference_wrapper<ColliderComponent>> collisionBuffer;

    std::variant<BoxColliderInfo, SphereColliderInfo, CapsuleColliderInfo, CylinderColliderInfo, ConeColliderInfo> info = BoxColliderInfo{HUGO::Vector3(-1.f, -1.f, -1.f)};
    HUGO::CollisionShapeID shapeID = HUGO::INVALID_SHAPE_ID;
    HUGO::CollisionObjectID objectID = HUGO::INVALID_COL_OBJ_ID;

    // ----------------------Setters----------------------
    void setRestitution(float nRestitution){
        restitution = nRestitution;
        flags = flags | UpdateFlags::RESTITUTION_UPDATED;
    }

    void setFriction(float nFriction){
        friction = nFriction;
        flags = flags | UpdateFlags::FRICTION_UPDATED;
    }

    void setPositionOffset(HUGO::Vector3 offset){
        positionOffset = offset;
        flags = flags | UpdateFlags::TRANSFORM_OFFSET_UPDATED;
    }

    void setRotationOffset(HUGO::Quaternion offset){
        rotationOffset = offset;
        flags = flags | UpdateFlags::TRANSFORM_OFFSET_UPDATED;
    }

    void setActive(bool isActive){
        active = isActive;
        flags = flags | UpdateFlags::ACTIVE_UPDATED;
    }

    // ----------------------Getters----------------------
    inline float getRestitution() const { return restitution; }
    inline float getFriction() const { return friction; }

    inline HUGO::Vector3 getPositionOffset() const { return positionOffset; }
    inline HUGO::Quaternion getRotationOffset() const { return rotationOffset; }

    inline bool getActive() const { return active; }

    inline std::vector<std::reference_wrapper<ColliderComponent>> getCollisionInfo() const { return collisionInfo; }

END_HCOMPONENT()

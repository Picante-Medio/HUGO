#pragma once

#include <variant>
#include "HugoMath.h"

namespace HUGO {

   // -------- Collider Types & Info --------
    enum class ColliderType 
    {
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        CONE
    };

    struct BoxColliderInfo
    {
        Vector3 halfExtents;
    };

    struct SphereColliderInfo
    {
        float r;
    };

    struct CapsuleColliderInfo
    {
        float r;
        float height;
    };

    struct CylinderColliderInfo
    {
        Vector3 halfExtents;
    };

    struct ConeColliderInfo
    {
        float r;
        float height;
    };

    using ColliderInfo = std::variant<BoxColliderInfo, SphereColliderInfo, CapsuleColliderInfo, CylinderColliderInfo, ConeColliderInfo>;
    

   // -------- Rigidbodies --------
    enum BodyType
    {
        DYNAMIC,
        STATIC,
        KINEMATIC
    };
}
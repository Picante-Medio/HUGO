#pragma once

#include <string>
#include <variant>
#include "Component.h"
#include "HugoTypes.h"
#include "HugoMath.h"

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

    friend class PhysicsSystem;
    // Shape's geometry is determined by the variant. Which struct is held can be checked with std::holds_alternative
    std::variant<BoxColliderInfo, SphereColliderInfo, CapsuleColliderInfo, CylinderColliderInfo, ConeColliderInfo> info;

private:
    HUGO::BulletShapeID id;
};

struct RigidBodyComponent: public Component
{
    float restitution;
    float friction;
    float mass;
    float damping;
    HUGO::Vector3 positionOffset;
    HUGO::Quaternion rotationOffset;

    HUGO::Vector3 linearVelocity;
    HUGO::Vector3 angularVelocity;

    // Forces can be applied throught rigidbody or external components to query which rigidbodies have force to be applied.
    // For this we need to query every rigidbody and a way to filter if the entity attached has or not the force component.
    // Since there's no way to do so at the moment, we choose the first option 

    HUGO::Vector3 forceToApplyThisFrame;
    HUGO::Vector3 torqueToApplyThisFrame;

    HUGO::Vector3 impulseToApplyThisFrame;
    HUGO::Vector3 impulseTorqueToApplyThisFrame;

    RigidBodyComponent(EntityID entity, Shape shape) 
        : Component(entity), rbShape(shape){};
    
    friend class PhysicsSystem;
private:

    // Once created shape shouldn't be modified
    Shape rbShape;
    HUGO::BulletObjectID objectID = HUGO::INVALID_BULLET_OBJECT_ID;
};
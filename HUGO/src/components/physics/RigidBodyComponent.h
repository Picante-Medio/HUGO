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
    friend class PhysicsSystem;
    
    RigidBodyComponent(EntityID entity, Shape shape);

    // Setters
    void setRestitution(float nRestitution);
    void setFriction(float nFriction);
    void setMass(float nMass);
    void setDamping(float nDamping);

    void applyImpulse(HUGO::Vector3 nImpulse); // Modifies linear velocity
    void applyTorqueImpulse(HUGO::Vector3 nImpulse); // Modifies angular velocity
    void applyForce(HUGO::Vector3 nForce); // Modifies linear acceleration
    void applyTorque(HUGO::Vector3 nTorque); // Modifies angular acceleration

    void setPositionOffset(HUGO::Vector3 offset);
    void setRotationOffset(HUGO::Quaternion offset);

    // Getters
    float getRestitution() const;
    float getFriction() const;
    float getMass() const;
    float getDamping() const;

    HUGO::Vector3 getLinearVelocity() const;
    HUGO::Vector3 getAngularVelocity() const;
    HUGO::Vector3 getLinearAcceleration() const;
    HUGO::Vector3 getAngularAcceleration() const;

    HUGO::Vector3 getPositionOffset() const;
    HUGO::Quaternion getRotationOffset() const;

private:
    float restitution;
    float friction;
    float mass;
    float damping;
    HUGO::Vector3 positionOffset;
    HUGO::Quaternion rotationOffset;

    HUGO::Vector3 linearVelocity;
    HUGO::Vector3 angularVelocity;
    HUGO::Vector3 linearAcceleration;
    HUGO::Vector3 angularAcceleration;

    // Once created shape shouldn't be modified
    Shape rbShape;
    HUGO::BulletObjectID objectID = HUGO::INVALID_BULLET_OBJECT_ID;
};
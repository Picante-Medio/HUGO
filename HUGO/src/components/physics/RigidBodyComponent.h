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

    friend class BulletSimulation;
    // Shape's geometry is determined by the variant. Which struct is held can be checked with std::holds_alternative
    std::variant<BoxColliderInfo, SphereColliderInfo, CapsuleColliderInfo, CylinderColliderInfo, ConeColliderInfo> info;

private:
    HUGO::CollisionShapeID id = HUGO::INVALID_SHAPE_ID;
};

struct RigidBodyComponent: public Component
{
    friend class BulletSimulation;
    
    RigidBodyComponent(EntityID entity, Shape shape, float mass = 0.f);

    // Setters
    void setRestitution(float nRestitution);
    void setFriction(float nFriction);
    void setMass(float nMass);
    void setLinearDamping(float nDamping);
    void setAngularDamping(float nDamping);
    void setLinearVelocity(HUGO::Vector3 nVelocity);
    void setAngularVelocity(HUGO::Vector3 nVelocity);

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
    float getLinearDamping() const;
    float getAngularDamping() const;

    HUGO::Vector3 getLinearVelocity() const;
    HUGO::Vector3 getAngularVelocity() const;

    HUGO::Vector3 getPositionOffset() const;
    HUGO::Quaternion getRotationOffset() const;

private:

    enum UpdateFlags{
        RESTITUTION_UPDATED = 1,
        FRICTION_UPDATED = 2,
        MASS_UPDATED = 4,
        DAMPING_UPDATED = 8,
        LINEAR_VELOCITY_UPDATED = 16,
        ANGULAR_VELOCITY_UPDATED = 32,
        IMPULSE_APPLIED = 64,
        TORQUE_IMPULSE_APPLIED = 128,
        FORCE_APPLIED = 256,
        TORQUE_APPLIED = 512,
        TRANSFORM_OFFSET_UPDATED = 1024
    };

    int flags;

    float restitution;
    float friction;
    float mass;
    float linearDamping;
    float angularDamping;
    HUGO::Vector3 positionOffset;
    HUGO::Quaternion rotationOffset;

    HUGO::Vector3 linearVelocity;
    HUGO::Vector3 angularVelocity;

    HUGO::Vector3 impulseApplied;
    HUGO::Vector3 torqueImpulseApplied;
    HUGO::Vector3 forceApplied;
    HUGO::Vector3 torqueApplied;

    // Once created shape shouldn't be modified
    Shape rbShape;
    HUGO::RigidBodyID objectID = HUGO::INVALID_RB_ID;
};
#pragma once

#include <string>
#include <variant>
#include "Component.h"
#include "HugoTypes.h"
#include "HugoMath.h"

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

HCOMPONENT(RigidBodyComponent)

    friend class BulletSimulation;
    friend class HUGODispatcher;
    
    RigidBodyComponent(EntityID entity, Shape shape, float mass = .0f)
    : Component(entity)
    , rbShape(shape)
    , flags(0)
    , mass(mass)
    , restitution(0.f) // default bullet value
    , friction(.5f) // default bullet value
    , linearDamping(0.f) // default bullet value
    , angularDamping(0.f) // default bullet value
    , positionOffset(HUGO::Vector3()) // default bullet value
    , rotationOffset(HUGO::Quaternion()) // default bullet value
    , linearVelocity(HUGO::Vector3()) // default bullet value
    , angularVelocity(HUGO::Vector3()) // default bullet value
    , active(true)
    , asleep(false)
    , linearSleepingThreshold(.8f)
    , angularSleepingThreshold(1.f)
    {}

    // ----------------------Setters----------------------
    void setRestitution(float nRestitution){
        restitution = nRestitution;
        flags = flags | UpdateFlags::RESTITUTION_UPDATED;
    }

    void setFriction(float nFriction){
        friction = nFriction;
        flags = flags | UpdateFlags::FRICTION_UPDATED;
    }

    void setMass(float nMass){
        mass = nMass;
        flags = flags | UpdateFlags::MASS_UPDATED;
    }

    void setLinearDamping(float nDamping){
        linearDamping = nDamping;
        flags = flags | UpdateFlags::DAMPING_UPDATED;
    }

    void setAngularDamping(float nDamping){
        angularDamping = nDamping;
        flags = flags | UpdateFlags::DAMPING_UPDATED;
    }

    void setLinearVelocity(HUGO::Vector3 nVelocity){
        linearVelocity = nVelocity;
        flags = flags | UpdateFlags::LINEAR_VELOCITY_UPDATED;
    }

    void setAngularVelocity(HUGO::Vector3 nVelocity){
        angularVelocity = nVelocity;
        flags = flags | UpdateFlags::ANGULAR_VELOCITY_UPDATED;
    }

    // Modifies linear velocity
    void applyImpulse(HUGO::Vector3 nImpulse){
        impulseApplied = nImpulse;
        flags = flags | UpdateFlags::IMPULSE_APPLIED;
    } 

    // Modifies angular velocity
    void applyTorqueImpulse(HUGO::Vector3 nImpulse){
        torqueImpulseApplied = nImpulse;
        flags = flags | UpdateFlags::TORQUE_IMPULSE_APPLIED;
    }

    // Modifies linear acceleration
    void applyForce(HUGO::Vector3 nForce){
        forceApplied = nForce;
        flags = flags | UpdateFlags::FORCE_APPLIED;
    }

    // Modifies angular acceleration
    void applyTorque(HUGO::Vector3 nTorque){
        torqueApplied = nTorque;
        flags = flags | UpdateFlags::TORQUE_APPLIED;
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

    void setAsleep(bool isAsleep){
        asleep = isAsleep;
        flags = flags | UpdateFlags::SLEEP_UPDATED;
    }

    void setLinearSleepingThreshold(float nThreshold){
        linearSleepingThreshold = nThreshold;
        flags = flags | UpdateFlags::SLEEPING_THRESHOLD_UPDATED;
    }

    void setAngularSleepingThreshold(float nThreshold){
        angularSleepingThreshold = nThreshold;
        flags = flags | UpdateFlags::SLEEPING_THRESHOLD_UPDATED;
    }

    // ----------------------Getters----------------------
    inline float getRestitution() const { return restitution; }
    inline float getFriction() const { return friction; }
    inline float getMass() const { return mass; }
    inline float getLinearDamping() const { return linearDamping; }
    inline float getAngularDamping() const { return angularDamping; }

    inline HUGO::Vector3 getLinearVelocity() const { return linearVelocity; }
    inline HUGO::Vector3 getAngularVelocity() const { return angularVelocity; }

    inline HUGO::Vector3 getPositionOffset() const { return positionOffset; }
    inline HUGO::Quaternion getRotationOffset() const { return rotationOffset; }

    inline bool getActive() const { return active; }
    inline bool getAsleep() const { return asleep; }
    inline float getLinearSleepingThreshold() const { return linearSleepingThreshold; }
    inline float getAngularSleepingThreshold() const { return angularSleepingThreshold; }

    inline std::vector<unsigned long int> getCollisionInfo() const { return collisionInfo; }

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
        TRANSFORM_OFFSET_UPDATED = 1024,
        ACTIVE_UPDATED = 2048,
        SLEEP_UPDATED = 4096,
        SLEEPING_THRESHOLD_UPDATED = 8192
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

    bool active;
    bool asleep;
    float linearSleepingThreshold;
    float angularSleepingThreshold;

    std::vector<unsigned long int> collisionInfo;

    // Once created shape shouldn't be modified
    Shape rbShape;
    HUGO::RigidBodyID objectID = HUGO::INVALID_RB_ID;

END_HCOMPONENT()
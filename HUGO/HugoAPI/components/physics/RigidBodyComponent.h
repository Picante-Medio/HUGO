#pragma once

#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoPhysics.h"

#include "HugoComponentMacros.h"

HCOMPONENT(RigidBodyComponent)
    // ----------------------Attributes----------------------
    enum UpdateFlags{
        MASS_UPDATED = 1,
        DAMPING_UPDATED = 2,
        LINEAR_VELOCITY_UPDATED = 4,
        ANGULAR_VELOCITY_UPDATED = 8,
        IMPULSE_APPLIED = 16,
        TORQUE_IMPULSE_APPLIED = 32,
        FORCE_APPLIED = 64,
        TORQUE_APPLIED = 128,
        SLEEP_UPDATED = 256,
        SLEEPING_THRESHOLD_UPDATED = 512,
        GRAVITY_UPDATED = 1024
    };

    int flags = 2047;

    DEFAULT_SERIALIZED_MEMBER(float, mass, 0.f)
    DEFAULT_SERIALIZED_MEMBER(float, linearDamping, 0.f)
    DEFAULT_SERIALIZED_MEMBER(float, angularDamping, 0.f)
    
    SERIALIZED_MEMBER(HUGO::Vector3, linearVelocity)
    SERIALIZED_MEMBER(HUGO::Vector3, angularVelocity)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, gravity, HUGO::Vector3(0.f, -9.8f, 0.f))
    
    HUGO::Vector3 impulseApplied = HUGO::Vector3();
    HUGO::Vector3 torqueImpulseApplied = HUGO::Vector3();
    HUGO::Vector3 forceApplied = HUGO::Vector3();
    HUGO::Vector3 torqueApplied = HUGO::Vector3();

    bool asleep = false;
    DEFAULT_SERIALIZED_MEMBER(bool, dontRotateWithPhysicsOnDynamic, false);
    DEFAULT_SERIALIZED_MEMBER(float, linearSleepingThreshold, 0.8f)
    DEFAULT_SERIALIZED_MEMBER(float, angularSleepingThreshold, 1.f)

    HUGO::CollisionObjectID objectID = HUGO::INVALID_COL_OBJ_ID;
    DEFAULT_SERIALIZED_MEMBER(HUGO::BodyType, type, HUGO::BodyType::DYNAMIC)

    // ----------------------Setters----------------------

    // must be declared on initialization. Otherwise setting this will do nothing at best and
    // will have un unpredictable outcome at worst
    void setBodyType(HUGO::BodyType nType){
        type = nType;
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
    
    void setGravity(HUGO::Vector3 nGravity){
        gravity = nGravity;
        flags = flags | UpdateFlags::GRAVITY_UPDATED;
    }
    
    // Modifies linear velocity
    void applyImpulse(HUGO::Vector3 nImpulse){
        impulseApplied = nImpulse;
        flags = flags | UpdateFlags::IMPULSE_APPLIED;
    } 

    // Modifies angular velocity
    void applyTorqueImpulse(HUGO::Vector3 nImpulse){
        torqueImpulseApplied += nImpulse;
        flags = flags | UpdateFlags::TORQUE_IMPULSE_APPLIED;
    }

    // Modifies linear acceleration
    void applyForce(HUGO::Vector3 nForce){
        forceApplied += nForce;
        flags = flags | UpdateFlags::FORCE_APPLIED;
    }

    // Modifies angular acceleration
    void applyTorque(HUGO::Vector3 nTorque){
        torqueApplied += nTorque;
        flags = flags | UpdateFlags::TORQUE_APPLIED;
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
    inline HUGO::BodyType getBodyType() const { return type; }
    inline float getMass() const { return mass; }
    inline float getLinearDamping() const { return linearDamping; }
    inline float getAngularDamping() const { return angularDamping; }

    inline HUGO::Vector3 getLinearVelocity() const { return linearVelocity; }
    inline HUGO::Vector3 getAngularVelocity() const { return angularVelocity; }
    inline HUGO::Vector3 geGravity() const { return gravity; }

    inline bool getAsleep() const { return asleep; }
    inline float getLinearSleepingThreshold() const { return linearSleepingThreshold; }
    inline float getAngularSleepingThreshold() const { return angularSleepingThreshold; }

END_HCOMPONENT()


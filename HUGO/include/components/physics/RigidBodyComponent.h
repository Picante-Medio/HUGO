#pragma once

#include "HugoTypes.h"
#include "HugoMath.h"

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
        SLEEPING_THRESHOLD_UPDATED = 512
    };

    int flags = 0;

    float mass = 0.f;
    float linearDamping = 0.f;
    float angularDamping = 0.f;

    HUGO::Vector3 linearVelocity = HUGO::Vector3();
    HUGO::Vector3 angularVelocity = HUGO::Vector3();

    HUGO::Vector3 impulseApplied = HUGO::Vector3();
    HUGO::Vector3 torqueImpulseApplied = HUGO::Vector3();
    HUGO::Vector3 forceApplied = HUGO::Vector3();
    HUGO::Vector3 torqueApplied = HUGO::Vector3();

    bool asleep = false;
    float linearSleepingThreshold = 0.8f;
    float angularSleepingThreshold = 1.f;

    HUGO::CollisionObjectID objectID = HUGO::INVALID_COL_OBJ_ID;

    // ----------------------Setters----------------------
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
    inline float getMass() const { return mass; }
    inline float getLinearDamping() const { return linearDamping; }
    inline float getAngularDamping() const { return angularDamping; }

    inline HUGO::Vector3 getLinearVelocity() const { return linearVelocity; }
    inline HUGO::Vector3 getAngularVelocity() const { return angularVelocity; }

    inline bool getAsleep() const { return asleep; }
    inline float getLinearSleepingThreshold() const { return linearSleepingThreshold; }
    inline float getAngularSleepingThreshold() const { return angularSleepingThreshold; }

END_HCOMPONENT()


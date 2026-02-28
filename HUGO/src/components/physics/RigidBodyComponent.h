#pragma once

#include <string>
#include "Component.h"
#include "HugoTypes.h"
#include "HugoMath.h"

enum ShapeType{
    BOX,
    SPHERE,
    CAPSULE,
    CYLINDER,
    CONE
};

struct _Shape{
private:
    HUGO::BulletShapeID id;
    ShapeType type;
};

// We declare private before public so we can use a pointer to any struct as a pointer to the Shape struct

struct BoxShapeInfo {
private:
    _Shape shape;

public:
    HUGO::Vector3 halfExtents;
};

struct SphereColliderInfo {
private:
    _Shape shape;

public:
    float r;
};

struct CapsuleColliderInfo {
private:
    _Shape shape;

public:
    float r;
    float height;
};

struct CylinderColliderInfo {
private:
    _Shape shape;

public:
    HUGO::Vector3 halfExtents;
};

struct ConeColliderInfo {
private:
    _Shape shape;

public:
    float r;
    float height;
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

    // pointer to bullet's RB for sync purposes. RBComponent is NOT the owner of the RB
    HUGO::simulationRB* simulationRigidBody = nullptr;

    RigidBodyComponent(EntityID entity, _Shape& shape) 
        : Component(entity){};
    
    friend class PhysicsSystem;
private:
    HUGO::BulletObjectID objectID = HUGO::INVALID_BULLET_OBJECT_ID;
};
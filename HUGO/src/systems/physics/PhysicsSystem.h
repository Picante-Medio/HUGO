#pragma once

#include "Ecs.h"
#include "System.h"
#include "TransformComponent.h"
#include "btBulletDynamicsCommon.h"
#include "PagedArray.h"
#include "HugoTypes.h"

class TransformComponent;
class RigidBodyComponent;
class Shape;
class PhysicsSimulation;

struct RigidBodyObject{
    HUGO::BulletObjectID id;
    btRigidBody* bulletRB = nullptr;
};

struct ShapeObject{
    HUGO::BulletShapeID id;
    btCollisionShape* bulletShape = nullptr;
};

class HUGOMotionState : public btMotionState{

    std::shared_ptr<TransformComponent> hugoTransform;
    btTransform btTransformRegister;

    // The center offset multiplies the bulletTransform to adjust the center of mass. By default it's the Identity
    // * This is a test. If the behaviour isn't the expected can be removed
    btTransform centerOfMassOffset;

public:

    HUGOMotionState(const btTransform initialPos, std::shared_ptr<TransformComponent> transform, btTransform offset = btTransform::getIdentity());

    void setHugoTransform(std::shared_ptr<TransformComponent> transform);
    void updateBulletTransform();

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
};

class PhysicsSystem : public System {

    std::shared_ptr<PhysicsSimulation> simulation;

    // Same logic as in RenderSystem. Credits to them, they did a really good job.
    PagedArray<RigidBodyObject, 128> rigidBodies;
    std::vector<HUGO::BulletObjectID> freeBulletObjectId;
    std::vector<std::size_t> objectIdToIndex;

    // We use ids to shapes so various rb can use the same shape
    PagedArray<ShapeObject, 128> shapes;
    std::vector<HUGO::BulletShapeID> freeBulletShapeId;
    std::vector<std::size_t> shapeIdToIndex;
    
    HUGO::BulletShapeID createShapeObject(Shape& shape);
    ShapeObject& getShapeObject(const HUGO::BulletShapeID id);
    void removeShapeObject(HUGO::BulletShapeID& id);
    void destroyShapeObject(ShapeObject& shObj);

    HUGO::BulletObjectID createRigidBodyObject(RigidBodyComponent& hgRb, const TransformComponent& tf);
    RigidBodyObject& getRigidBodyObject(const HUGO::BulletObjectID id);
    void removeRigidBodyObject(HUGO::BulletObjectID& id);
    void destroyRigidBodyObject(RigidBodyObject& RbObj);

public:
    PhysicsSystem(std::shared_ptr<Scene> scene, std::shared_ptr<PhysicsSimulation> sim);
    ~PhysicsSystem();

    void update(double dt) override;
    void fixedUpdate(double dt) override;
};
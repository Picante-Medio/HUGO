#pragma once

#include "Ecs.h"
#include "System.h"
#include "TransformComponent.h"
#include "../bullet3-src/src/btBulletDynamicsCommon.h"

class HUGOMotionState : public btMotionState{

    std::shared_ptr<TransformComponent> hugoTransform;
    btTransform btTransformRegister;

    // The center offset multiplies the bulletTransform to adjust the center of mass. By default its the Identity
    // * This is a test. If the behaviour isn't the expected can be removed
    btTransform centerOfMassOffset;

public:

    HUGOMotionState(const btTransform initialPos, std::shared_ptr<TransformComponent> transform, btTransform offset = btTransform::getIdentity());

    void setHugoTransform(std::shared_ptr<TransformComponent> transform);
    void updateBulletTransform(TransformComponent newPos);

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
};

class PhysicsSystem : public System {

    std::shared_ptr<btDiscreteDynamicsWorld> btWorld;
    std::shared_ptr<btDefaultCollisionConfiguration> collisionConfig;
    std::shared_ptr<btCollisionDispatcher> dispatcher;
    std::shared_ptr<btDbvtBroadphase> overlappingPairCache;
    std::shared_ptr<btSequentialImpulseConstraintSolver> solver;

public:
    PhysicsSystem(std::shared_ptr<Scene> scene);

    void update(double dt) override;
    void fixedUpdate(double dt) override;
};
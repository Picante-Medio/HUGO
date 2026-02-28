#pragma once

#include "Ecs.h"
#include "System.h"
#include "TransformComponent.h"
#include "btBulletDynamicsCommon.h"

class HUGOMotionState : public btMotionState{

    std::shared_ptr<TransformComponent> hugoTransform;
    btTransform btTransformRegister;

    // The center offset multiplies the bulletTransform to adjust the center of mass. By default it's the Identity
    // * This is a test. If the behaviour isn't the expected can be removed
    btTransform centerOfMassOffset;

public:

    HUGOMotionState(const btTransform initialPos, std::shared_ptr<TransformComponent> transform, btTransform offset = btTransform::getIdentity());

    void setHugoTransform(std::shared_ptr<TransformComponent> transform);
    void updateBulletTransform(const TransformComponent& newPos);

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
};

class PhysicsSystem : public System {
    

public:
    PhysicsSystem(std::shared_ptr<Scene> scene);
    ~PhysicsSystem();

    void update(double dt) override;
    void fixedUpdate(double dt) override;
};
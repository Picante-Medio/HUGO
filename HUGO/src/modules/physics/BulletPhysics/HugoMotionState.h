#pragma once
#include <memory>
#include "btBulletDynamicsCommon.h"
#include "Scene.h"

class HUGOMotionState : public btMotionState{

    unsigned long int entityID;
    std::shared_ptr<Scene> hugoScene;
    btTransform btTransformRegister;

    // The center offset multiplies the bulletTransform to adjust the center of mass. By default it's the Identity
    // * This is a test. If the behaviour isn't the expected can be removed
    btTransform centerOfMassOffset;

public:

    HUGOMotionState(const btTransform initialPos, std::shared_ptr<Scene> scene, unsigned long int id, btTransform offset = btTransform::getIdentity());

    void updateBulletTransform();

    void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;
};

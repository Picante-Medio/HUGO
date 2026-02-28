#include <iostream>

#include "PhysicsSystem.h"
#include "HugoMath.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysicsTags.h"
#include "Scene.h"

// ----------------------HUGOMotionState----------------------

HUGOMotionState::HUGOMotionState(const btTransform initialPos, std::shared_ptr<TransformComponent> transform, btTransform offset)
: btTransformRegister(initialPos)
, hugoTransform(transform)
, centerOfMassOffset(offset)
{

}

void HUGOMotionState::setHugoTransform(std::shared_ptr<TransformComponent> transform)
{
    hugoTransform = transform;
}
    
void HUGOMotionState::updateBulletTransform(const TransformComponent& newPos)
{
    btTransform auxTransform = btTransform(
                               newPos.rotation.toBullet(), 
                               newPos.position.toBullet());

    btTransformRegister = auxTransform;
}

void HUGOMotionState::getWorldTransform(btTransform& worldTrans) const
{
    worldTrans = btTransformRegister * centerOfMassOffset;
}
    
void HUGOMotionState::setWorldTransform(const btTransform& worldTrans)
{
    btTransformRegister = worldTrans * centerOfMassOffset;
    if(hugoTransform.use_count() == 0){
        return;
    }

    btQuaternion rot = btTransformRegister.getRotation();
    hugoTransform->rotation = HUGO::Quaternion(rot.w(), rot.x(), rot.y(), rot.z());
    btVector3 pos = btTransformRegister.getOrigin();
    hugoTransform->position = HUGO::Vector3(pos.x(), pos.y(), pos.z());
}

// ----------------------PhysicsSystem----------------------

PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene)
: System(scene)
{

}

PhysicsSystem::~PhysicsSystem() {

}

void PhysicsSystem::update(double dt) {
}

void PhysicsSystem::fixedUpdate(double dt) {
    // add rigidBodies to simulation

    // update kinematic bodies in the simulation
    for(auto& pack: scene->query<KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        HUGOMotionState* motionState = static_cast<HUGOMotionState*>(rb.simulationRigidBody->getMotionState());
        motionState->updateBulletTransform(transform);
    }
}
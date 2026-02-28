#include <iostream>

#include "PhysicsSystem.h"
#include "HugoMath.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysicsTags.h"
#include "Scene.h"

#pragma region HUGOMotionState
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
#pragma endregion

#pragma region PhysicsSystem
// ----------------------PhysicsSystem----------------------

PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene)
: System(scene)
{

}

PhysicsSystem::~PhysicsSystem() 
{

}

void PhysicsSystem::update(double dt) 
{

}

void PhysicsSystem::fixedUpdate(double dt) 
{
    // Add rigidBodies to simulation and Update if neccessary
    // Static rigidBodies 
    for(auto& pack: scene->query<StaticRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();

        if (rb.objectID == HUGO::INVALID_BULLET_OBJECT_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb);
        }
        
    }
    
    // Kinematic rigidBodies
    for(auto& pack: scene->query<NewRBTag, KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_OBJECT_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb);
        }

        // Update BtRB from HugoRB
        
    }
    
    // Dynamic rigidBodies
    for(auto& pack: scene->query<NewRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_OBJECT_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb);
        }

        // Update BtRB from HugoRB
        
    }

    // update kinematic bodies in the Motion State
    for(auto& pack: scene->query<KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        HUGOMotionState* motionState = static_cast<HUGOMotionState*>(rb.simulationRigidBody->getMotionState());
        motionState->updateBulletTransform(transform);
    }
}

HUGO::BulletObjectID PhysicsSystem::createRigidBodyObject(const RigidBodyComponent hgRb)
{
    // Create a RigidBodyObject and assign an id. Must have a way to determine if it's static, kinematic or dynamic
}
    
RigidBodyObject& PhysicsSystem::getRigidBodyObject(const HUGO::BulletObjectID)
{
    // Return RigidBodyObject of the given id
}
    
void PhysicsSystem::removeRigidBodyObject(const HUGO::BulletObjectID)
{
    // Remove RigidBodyObject from the list of registered ids
}
    
void PhysicsSystem::destroyRigidBodyObject(RigidBodyObject& RbObj)
{
    // Destroy contents of the RigidBodyObjects
}
#pragma endregion
#include <iostream>

#include "PhysicsSystem.h"
#include "HugoMath.h"
#include "TransformComponent.h"
#include "PhysicsTags.h"
#include "Scene.h"
#include "RigidBodyComponent.h"
#include "PhysicsSimulation.h"

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
    
void HUGOMotionState::updateBulletTransform()
{
    // Uses associated HUGO::TransformComponent to update itself
    btTransform auxTransform = btTransform(
                               hugoTransform->rotation.toBullet(), 
                               hugoTransform->position.toBullet());

    btTransformRegister = auxTransform * centerOfMassOffset;
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

PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene, std::shared_ptr<PhysicsSimulation> sim)
: System(scene)
, simulation(sim)
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
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::STATIC);
        }
        
    }
    
    // Kinematic rigidBodies (creation and update)
    for(auto& pack: scene->query<KinematicRBTag, RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_OBJECT_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::KINEMATIC);
        }
        
        HUGOMotionState* motionState = static_cast<HUGOMotionState*>(getRigidBodyObject(rb.objectID).bulletRB->getMotionState());
        motionState->updateBulletTransform();
    }
    
    // Dynamic rigidBodies
    for(auto& pack: scene->query<RigidBodyComponent, TransformComponent>()) {
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();
        TransformComponent& transform = pack.get<TransformComponent&>();
        
        if (rb.objectID == HUGO::INVALID_BULLET_OBJECT_ID) {
            //create a new RigidBodyObject
            rb.objectID = createRigidBodyObject(rb, transform, bodyType::DYNAMIC);
        }
        
    }
}
#pragma endregion

#pragma region ShapeObject
HUGO::BulletShapeID PhysicsSystem::createShapeObject(Shape& shape)
{

    btCollisionShape* resShape = nullptr;
    if(std::holds_alternative<BoxColliderInfo>(shape.info)){
        resShape = new btBoxShape(std::get<BoxColliderInfo>(shape.info).halfExtents.toBullet());
    }
    else if(std::holds_alternative<SphereColliderInfo>(shape.info)){
        resShape = new btSphereShape(std::get<SphereColliderInfo>(shape.info).r);
    }
    else if(std::holds_alternative<CapsuleColliderInfo>(shape.info)){
        CapsuleColliderInfo aux = std::get<CapsuleColliderInfo>(shape.info);
        resShape = new btCapsuleShape(aux.r, aux.height);
    }
    else if(std::holds_alternative<CylinderColliderInfo>(shape.info)){
        resShape = new btCylinderShape(std::get<CylinderColliderInfo>(shape.info).halfExtents.toBullet());
    }
    else{ // Cone
        ConeColliderInfo aux = std::get<ConeColliderInfo>(shape.info);
        resShape = new btConeShape(aux.r, aux.height);
    }

    HUGO::BulletShapeID id;
    //reuse id
    if(!freeBulletShapeId.empty()){ 
        id= freeBulletShapeId.back();
        freeBulletShapeId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::BulletShapeID>(shapeIdToIndex.size());
        shapeIdToIndex.push_back(0);
    }

    //create Bullet Shape
    ShapeObject obj = {id, resShape};
    shape.id = id;

    //store object
    std::size_t index = shapeObjects.push_back(std::move(obj));
    shapeIdToIndex[id] = index;

    return id;
}

ShapeObject& PhysicsSystem::getShapeObject(const HUGO::BulletShapeID id)
{
    return shapeObjects[shapeIdToIndex[id]];
}

void PhysicsSystem::removeShapeObject(HUGO::BulletShapeID& id)
{
    // Must make sure any rigidbody that used the shape to be deleted doesn't exist anymore
    if (id != HUGO::INVALID_BULLET_SHAPE_ID){
        
        std::size_t index = shapeIdToIndex[id];
        std::size_t lastIndex = shapeObjects.size() - 1;

        destroyShapeObject(shapeObjects[index]);

        if (index != lastIndex) {
            // mover último objeto al hueco
            shapeObjects[index] = std::move(shapeObjects[lastIndex]);
            HUGO::BulletShapeID movedID = shapeObjects[index].id;
            shapeIdToIndex[movedID] = index;
        }

        shapeObjects.remove(lastIndex);
        freeBulletShapeId.push_back(id);
        id = HUGO::INVALID_BULLET_SHAPE_ID;
    }
}

void PhysicsSystem::destroyShapeObject(ShapeObject& shObj)
{
    if(shObj.bulletShape){
        delete shObj.bulletShape;
        shObj.bulletShape = nullptr;
    }
}
#pragma endregion

#pragma region BulletObject
HUGO::BulletObjectID PhysicsSystem::createRigidBodyObject(RigidBodyComponent& hgRb, const TransformComponent& tf, bodyType type)
{

    // Create/reuse shape object
    btCollisionShape* shape = nullptr;
    if(hgRb.rbShape.id != HUGO::INVALID_BULLET_SHAPE_ID){
        shape = getShapeObject(hgRb.rbShape.id).bulletShape;
    }
    else{
        shape = getShapeObject(createShapeObject(hgRb.rbShape)).bulletShape;
    }

    // Create state
    HUGOMotionState* state = new HUGOMotionState(btTransform(), std::make_shared<TransformComponent>(tf));
    state->updateBulletTransform();

    // Create bullet rb
    btRigidBody* rb = new btRigidBody(hgRb.mass, state, shape);

    // Set flags for static, dynamic or kinematic
    switch(type){
        case bodyType::STATIC:
            rb->setFlags(btCollisionObject::CF_STATIC_OBJECT);
            break;
        case bodyType::KINEMATIC:
            rb->setFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
            break;
        case bodyType::DYNAMIC:
        default:
            break;
    }

    HUGO::BulletObjectID id;
    //reuse id
    if(!freeBulletObjectId.empty()){ 
        id= freeBulletObjectId.back();
        freeBulletObjectId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::BulletObjectID>(objectIdToIndex.size());
        objectIdToIndex.push_back(0);
    }
    
    //create RigidBodyObject
    RigidBodyObject obj = {id, rb};

    //store object
    std::size_t index = rigidBodies.push_back(std::move(obj));
    objectIdToIndex[id] = index;

    return id;
}
    
RigidBodyObject& PhysicsSystem::getRigidBodyObject(const HUGO::BulletObjectID id)
{
    return rigidBodies[objectIdToIndex[id]];
}
    
void PhysicsSystem::removeRigidBodyObject(HUGO::BulletObjectID& id)
{
    if (id != HUGO::INVALID_BULLET_OBJECT_ID){
        
        std::size_t index = objectIdToIndex[id];
        std::size_t lastIndex = rigidBodies.size() - 1;

        destroyRigidBodyObject(rigidBodies[index]);

        if (index != lastIndex) {
            // mover último objeto al hueco
            rigidBodies[index] = std::move(rigidBodies[lastIndex]);
            HUGO::BulletObjectID movedID = rigidBodies[index].id;
            objectIdToIndex[movedID] = index;
        }

        rigidBodies.remove(lastIndex);
        freeBulletObjectId.push_back(id);
        id = HUGO::INVALID_BULLET_OBJECT_ID;
    }
}
    
void PhysicsSystem::destroyRigidBodyObject(RigidBodyObject& RbObj)
{
    btRigidBody* body = RbObj.bulletRB;

    if(body){
        if (body->getMotionState())
	    {
		    delete body->getMotionState();
	    }
        btDynamicsWorld* btWorld = static_cast<btDynamicsWorld*>(simulation->getSimulation());

	    btWorld->removeCollisionObject(body);
	    delete body;
        RbObj.bulletRB = nullptr;
    }
}
#pragma endregion
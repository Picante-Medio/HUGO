#pragma once

#include "PagedArray.h"
#include "btBulletDynamicsCommon.h"
#include "PhysicsSimulation.h"
#include "HugoTypes.h"

class TransformComponent;
class RigidBodyComponent;
class Shape;

struct RigidBodyObject{
    HUGO::BulletObjectID id;
    btRigidBody* bulletRB = nullptr;
};

struct ShapeObject{
    HUGO::BulletShapeID id;
    btCollisionShape* bulletShape = nullptr;
};

class BulletSimulation : public PhysicsSimulation {

    // ------ Simulation & configuration ------

    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btDbvtBroadphase* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDynamicsWorld* btWorld;


    // ------ Rigidbody & collision shapes management ------

    enum bodyType{
        DYNAMIC,
        STATIC,
        KINEMATIC
    };
    
    // Same logic as in RenderSystem. Credits to them, they did a really good job.
    PagedArray<RigidBodyObject, 128> rigidBodies;
    std::vector<HUGO::BulletObjectID> freeBulletRBObjectId;
    std::vector<std::size_t> rbIdToIndex;

    // We use ids to shapes so various rb can use the same shape
    PagedArray<ShapeObject, 128> collisionShapes;
    std::vector<HUGO::BulletShapeID> freeBulletShapeId;
    std::vector<std::size_t> shapeIdToIndex;
    
    HUGO::BulletShapeID createShapeObject(Shape& shape);
    ShapeObject& getShapeObject(const HUGO::BulletShapeID id);
    void removeShapeObject(HUGO::BulletShapeID& id);
    void destroyShapeObject(ShapeObject& shObj);

    HUGO::BulletObjectID createRigidBodyObject(RigidBodyComponent& hgRb, const TransformComponent& tf, bodyType type);
    RigidBodyObject& getRigidBodyObject(const HUGO::BulletObjectID id);
    void removeRigidBodyObject(HUGO::BulletObjectID& id);
    void destroyRigidBodyObject(RigidBodyObject& RbObj);

    // Creates and adds rigidbodies to simulation. Updates kinematic bodies.
    void syncRigidBodies();

    public:
    
    BulletSimulation(std::shared_ptr<Scene> scene);
    ~BulletSimulation();

    void* getSimulation() override;
    void stepSimulation(double dt);
};
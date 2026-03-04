#pragma once

#include "PagedArray.h"
#include "btBulletDynamicsCommon.h"
#include "IPhysicsSimulation.h"
#include "HugoTypes.h"

class TransformComponent;
class RigidBodyComponent;
class Shape;

struct RigidBodyObject{
    HUGO::BulletRigidbodyID id;
    btRigidBody* bulletRB = nullptr;
};

struct ShapeObject{
    HUGO::BulletShapeID id;
    btCollisionShape* bulletShape = nullptr;
};

class BulletSimulation : public IPhysicsSimulation {

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
    std::vector<HUGO::BulletRigidbodyID> freeBulletRBObjectId;
    std::vector<std::size_t> rbIdToIndex;

    // We use ids to shapes so various rb can use the same shape
    PagedArray<ShapeObject, 128> collisionShapes;
    std::vector<HUGO::BulletShapeID> freeBulletShapeId;
    std::vector<std::size_t> shapeIdToIndex;
    
    HUGO::BulletShapeID createShapeObject(Shape& shape);
    ShapeObject& getShapeObject(const HUGO::BulletShapeID id);
    void removeShapeObject(HUGO::BulletShapeID& id);
    void destroyShapeObject(ShapeObject& shObj);

    HUGO::BulletRigidbodyID createRigidBodyObject(RigidBodyComponent& hgRb, const TransformComponent& tf, bodyType type);
    RigidBodyObject& getRigidBodyObject(const HUGO::BulletRigidbodyID id);
    void removeRigidBodyObject(HUGO::BulletRigidbodyID& id);
    void destroyRigidBodyObject(RigidBodyObject& RbObj);

    // Check for updates on a specific RB
    void checkFlags(RigidBodyComponent& hgRB);

    // @brief Creates and adds rigidbodies to simulation. Updates kinematic bodies.
    void syncBodiesToBullet();

    void stepSimulation(double dt);

    // Already performed by the motion state
    // void syncBodiesToGame(); 

public:
    
    BulletSimulation(std::shared_ptr<Scene> scene);
    ~BulletSimulation();

    // void* getSimulation() override;

    // Sync bodies to bullet, step simulation, sync bodies back to game
    void performSimulation(double dt) override;
};
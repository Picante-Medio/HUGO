#pragma once

#include "PagedArray.h"
#include "btBulletDynamicsCommon.h"
#include "IPhysicsSimulation.h"
#include "HugoTypes.h"

class TransformComponent;
class RigidBodyComponent;
class Shape;

struct RigidBodyObject{
    HUGO::RigidBodyID id;
    btRigidBody* bulletRB = nullptr;
};

struct ShapeObject{
    HUGO::CollisionShapeID id;
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
    
    // Same logic as in RenderSystem. Credits to them, they did a really good job.
    PagedArray<RigidBodyObject, 128> rigidBodies;
    std::vector<HUGO::RigidBodyID> freeBulletRBObjectId;
    std::vector<std::size_t> rbIdToIndex;

    // We use ids to shapes so various rb can use the same shape
    PagedArray<ShapeObject, 128> collisionShapes;
    std::vector<HUGO::CollisionShapeID> freeBulletShapeId;
    std::vector<std::size_t> shapeIdToIndex;
    
    HUGO::CollisionShapeID createShapeObject(Shape& shape);
    ShapeObject& getShapeObject(const HUGO::CollisionShapeID id);
    void removeShapeObject(HUGO::CollisionShapeID& id);
    void destroyShapeObject(ShapeObject& shObj);

    HUGO::RigidBodyID createRigidBodyObject(RigidBodyComponent& hgRb, TransformComponent& tf, HUGO::BodyType type);
    RigidBodyObject& getRigidBodyObject(const HUGO::RigidBodyID id);
    void removeRigidBodyObject(HUGO::RigidBodyID& id);
    void destroyRigidBodyObject(RigidBodyObject& RbObj);

    // Check for updates on a specific RB
    void checkFlags(RigidBodyComponent& hgRB);

    // @brief Creates and adds rigidbodies to simulation. Updates kinematic bodies.
    void syncBodiesToBullet();

    void stepSimulation(double dt);

    // @brief Updates the asleep parameter of the RB using their bullet counterpart
    void syncBodiesToGame(); 

public:
    
    BulletSimulation(std::shared_ptr<Scene> scene);
    ~BulletSimulation();

    void destroySimulation();

    // Sync bodies to bullet, step simulation, sync bodies back to game
    void performSimulation(double dt) override;
};
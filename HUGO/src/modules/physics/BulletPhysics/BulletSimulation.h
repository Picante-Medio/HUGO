#pragma once

#include "PagedArray.h"
#include "btBulletDynamicsCommon.h"
#include "IPhysicsSimulation.h"
#include "HugoTypes.h"
#include "EcsTypes.h"

struct TransformComponent;
struct RigidBodyComponent;
struct ColliderComponent;
struct Shape;

struct CollisionObject{
    std::shared_ptr<Scene> scene;
    HUGO::CollisionObjectID id;
    EntityID hgId;
    btCollisionObject* bulletRB = nullptr;
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
    int numSubSteps;

    btIDebugDraw* debugDrawer;

    // ------ Rigidbody & collision shapes management ------
    
    // Same logic as in RenderSystem. Credits to them, they did a really good job.
    PagedArray<CollisionObject, 128> rigidBodies;
    std::vector<HUGO::CollisionObjectID> freeBulletRBObjectId;
    std::vector<std::size_t> rbIdToIndex;

    // We use ids to shapes so various rb can use the same shape
    PagedArray<ShapeObject, 128> collisionShapes;
    std::vector<HUGO::CollisionShapeID> freeBulletShapeId;
    std::vector<std::size_t> shapeIdToIndex;
    
    HUGO::CollisionShapeID createShapeObject(ColliderComponent& col);
    ShapeObject& getShapeObject(const HUGO::CollisionShapeID id);
    void removeShapeObject(HUGO::CollisionShapeID& id);
    void destroyShapeObject(ShapeObject& shObj);
    btCollisionShape* getorCreateShape(HUGO::CollisionShapeID& id, ColliderComponent& hgCol);

    HUGO::CollisionObjectID createColliderObject(ColliderComponent& hgCol);
    HUGO::CollisionObjectID upgradeToRigidBodyObject(ColliderComponent& hgCol, RigidBodyComponent& hgRb);
    CollisionObject& getCollisionObject(const HUGO::CollisionObjectID id);
    void removeRigidBodyObject(HUGO::CollisionObjectID& id);
    void removeColliderObject(HUGO::CollisionObjectID& id);

    // Check for updates on a specific RB or collider
    void checkRigidBodyFlags(RigidBodyComponent& hgRB);
    void checkColliderFlags(ColliderComponent& hgCol);
    void checkTransformFlags(const ColliderComponent& collider, const TransformComponent& transform);

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

    void debugDraw() override;
};

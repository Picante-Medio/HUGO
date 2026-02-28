#include "btBulletDynamicsCommon.h"

class PhysicsSimulation {
    
    public:
    virtual void* getSimulation() = 0;
    virtual void stepSimulation(double dt) = 0;
};

class BulletSimulation : public PhysicsSimulation {

    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btDbvtBroadphase* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDynamicsWorld* btWorld;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;

    public:
    
    BulletSimulation();
    ~BulletSimulation();

    void* getSimulation() override;
    void stepSimulation(double dt);
};
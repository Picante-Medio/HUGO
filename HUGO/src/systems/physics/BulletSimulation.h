#include "btBulletDynamicsCommon.h"

class BulletSimulation {

    btDefaultCollisionConfiguration* collisionConfig;
    btCollisionDispatcher* dispatcher;
    btDbvtBroadphase* overlappingPairCache;
    btSequentialImpulseConstraintSolver* solver;

    btDynamicsWorld* btWorld;

    BulletSimulation();
    ~BulletSimulation();

public:

    btDynamicsWorld* getSimulation();
    void stepSimulation(double dt);
};
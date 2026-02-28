#include "PhysicsSimulation.h"

BulletSimulation::BulletSimulation() {
    // We are using bullet's default collision configuration. Custom configuration is recommended for more complexity
    collisionConfig = new btDefaultCollisionConfiguration();

    // We are using single-thread dispatcher (narrowphase)
    dispatcher = new btCollisionDispatcher(collisionConfig);

    // We set the broadphase as a DBVT (tree/hierarchy broadphase)
    overlappingPairCache = new btDbvtBroadphase();

    // We are using single-thread constraint solver
    solver = new btSequentialImpulseConstraintSolver();

    // Finally we create the world and set its gravity
    btWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
    btWorld->setGravity(btVector3(0, -9.8, 0));
}

BulletSimulation::~BulletSimulation() {
    // remove and delete rigidbodies
    for (int i = btWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = btWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
        
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		btWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

    delete btWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfig;

}

void* BulletSimulation::getSimulation() {
    return static_cast<void*>(btWorld);
}

void BulletSimulation::stepSimulation(double dt) {
    btWorld->stepSimulation(dt);
}

void BulletSimulation::addShape(btCollisionShape* collisionShape) {
    collisionShapes.push_back(collisionShape);
}
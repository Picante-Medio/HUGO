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

    // btBoxShape* testshape = new btBoxShape({1, 1, 1});
    // btMotionState* testmstate = new btDefaultMotionState();

    //TODO Descomentar y comprobar si los cuerpos se mantienen al destruir el mundo o no (para saber si destruirlos a mano)
    //testrb = new btRigidBody(1.0, testmstate, testshape);
    //btWorld->addRigidBody(testrb);

    //delete btWorld;

    //testrb->activate();
    //std::cout << "Se ha accedido al rigidbody sin que exista el mundo";
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
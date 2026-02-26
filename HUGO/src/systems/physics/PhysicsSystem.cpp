#include "PhysicsSystem.h"
#include "HugoMath.h"
#include <iostream>

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
    
void HUGOMotionState::updateBulletTransform(TransformComponent newPos)
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

// ----------------------PhysicsSystem----------------------

PhysicsSystem::PhysicsSystem(std::shared_ptr<Scene> scene)
: System(scene)
{

    //ESTA PARTE DE AQUI ABAJO ES LA QUE HAY QUE USAR PARA PROBAR BULLET

        // // We are using bullet's default collision configuration. Custom configuration is recommended for more complexity
        // btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

        // // We are using monoThread dispatcher
        // btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

        // // We set the broadphase as a DBVT (tree/hierarchy broadphase), as we have seen in the last quadrimester
        // btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

        // // We are using monoThread constraint solver
        // btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

        // // Finally we create the world and set its gravity
        // btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(
        //     dispatcher,
        //     overlappingPairCache,
        //     solver,
        //     collisionConfiguration
        // );
        // //btDiscreteDynamicsWorld* btWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
        // dynamicsWorld->setGravity(btVector3(0, -10, 0));


    //====================================================

    // A partir de aquí hay cosas mal, cuando se declara el dispatche se le pasa a su constructora 
    // un puntero inteligente (conllisionConfig), el constructor de btCollisionDispatche no acepta eso
    // no lo borro por si alguien va a necesitar ver eso 
    // - Marco

        // // We are using bullet's default collision configuration. Custom configuration is recommended for more complexity
        // collisionConfig = std::make_shared<btDefaultCollisionConfiguration>();

        // // We are using monoThread dispatcher
        // dispatcher = std::make_shared<btCollisionDispatcher>(collisionConfig);

        // // We set the broadphase as a DBVT (tree/hierarchy broadphase), as we have seen in the last quadrimester
        // overlappingPairCache = std::make_shared<btDbvtBroadphase>();

        // // We are using monoThread constraint solver
        // solver = std::make_shared<btSequentialImpulseConstraintSolver>();

        // // Finally we create the world and set its gravity
        // btWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher, overlappingPairCache, solver, collisionConfig);
        // //btDiscreteDynamicsWorld* btWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfig);
        // btWorld->setGravity(btVector3(0, -9.8, 0));

        // btBoxShape* testshape = new btBoxShape({1, 1, 1});
        // btMotionState* testmstate = new btDefaultMotionState();

        // //TODO Descomentar y comprobar si los cuerpos se mantienen al destruir el mundo o no (para saber si destruirlos a mano)
        // //testrb = new btRigidBody(1.0, testmstate, testshape);
        // //btWorld->addRigidBody(testrb);

        // //delete btWorld;

        // //testrb->activate();
        // //std::cout << "Se ha accedido al rigidbody sin que exista el mundo";
}

void PhysicsSystem::update(double dt) {
    
}

void PhysicsSystem::fixedUpdate(double dt) {
    
}
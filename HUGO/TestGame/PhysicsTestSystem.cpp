#include "PhysicsTestSystem.h"
#include "PhysicsTags.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

PhysicsTestSystem::~PhysicsTestSystem() {

}

void PhysicsTestSystem::init(){
        
    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();

    scene->addComponent<TransformComponent>(e1, [](TransformComponent& transform){
<<<<<<< HEAD
        transform.setPosition(HUGO::Vector3(1.f, 0.f, 40.f));
        transform.setScale(HUGO::Vector3(1., 1.f, 1.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(45.f)), HUGO::Vector3(0,0,1)));
=======
        transform.position = HUGO::Vector3(1.f, -3.f, 40.f);
        transform.scale = HUGO::Vector3(1., 1.f, 1.f);
        //transform.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(45.f)), HUGO::Vector3(0,0,1));
>>>>>>> 284103a (mejorada simulacion fisica y avances con bug)
    });

    scene->addComponent<ColliderComponent>(e1, [](ColliderComponent& collider){
        collider.info = BoxColliderInfo{HUGO::Vector3(5.f, 0.5f, 1.f)};
    });

    scene->addComponent<RigidBodyComponent>(e1, [](RigidBodyComponent& rb){
        // rb->setShape()
        // rb.setMass(5.f);
    });
    // scene->addComponent<DynamicRBTag>(e1);

    scene->addComponent<StaticRBTag>(e1);

    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();

    scene->addComponent<TransformComponent>(e2, [](TransformComponent& transform){
<<<<<<< HEAD
        transform.setPosition(HUGO::Vector3(0.f, 5, 40.f));
        transform.setScale(HUGO::Vector3(1.f, 1.f, 1.f));
=======
        transform.position = HUGO::Vector3(1.25f, 15, 40.f);
        transform.scale = HUGO::Vector3(1.f, 1.f, 1.f);
        transform.rotation = HUGO::Quaternion(HUGO::Radian(HUGO::Degree(45)), HUGO::Vector3(0, 0, 1));
>>>>>>> 284103a (mejorada simulacion fisica y avances con bug)
    });
    //scene->addComponent<TransformChangedTag>(e2);

    scene->addComponent<ColliderComponent>(e2, [](ColliderComponent& collider){
        collider.info = CylinderColliderInfo{HUGO::Vector3(.5f, 1, 1)};
    });

    scene->addComponent<RigidBodyComponent>(e2, [](RigidBodyComponent& rb){
        rb.setMass(5.f);
        // rb->setShape()
    });

    scene->addComponent<DynamicRBTag>(e2);
    
    EntityID e3 = scene->createEntity();
    scene->addComponent<TransformComponent>(e3,[](TransformComponent& transform){
        transform.position = HUGO::Vector3(0.f, 5, 40.f);
    });
    
    scene->addComponent<ColliderComponent>(e3, [](ColliderComponent& col){
        col.info = BoxColliderInfo{HUGO::Vector3(1.f, 1.f, 1.f)};
    });

    scene->addComponent<RigidBodyComponent>(e3, [](RigidBodyComponent& rb){
        rb.setMass(5.f);
    });

    scene->addComponent<DynamicRBTag>(e3);
}

void PhysicsTestSystem::onLoad(){
    
}

void PhysicsTestSystem::fixedUpdate(double dt) {
    
}

float rate = 0.2f;
float t = 0;
void PhysicsTestSystem::update(double dt) {

}

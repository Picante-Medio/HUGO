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
        transform.setPosition(HUGO::Vector3(1.f, 0.f, 40.f));
        transform.setScale(HUGO::Vector3(1., 1.f, 1.f));
        transform.setRotation(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(45.f)), HUGO::Vector3(0,0,1)));
    });

    scene->addComponent<ColliderComponent>(e1, [](ColliderComponent& collider){
        collider.info = SphereColliderInfo{1.f};
    });

    scene->addComponent<RigidBodyComponent>(e1);
    // rb->setShape()
    // rb.setMass(5.f);
    // scene->addComponent<DynamicRBTag>(e1);

    scene->addComponent<StaticRBTag>(e1);

    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();

    scene->addComponent<TransformComponent>(e2, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0.f, 5, 40.f));
        transform.setScale(HUGO::Vector3(1.f, 1.f, 1.f));
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
    
    // Entidad 3: Cuerpo kinemático
    // EntityID e3 = scene->createEntity();
    // scene->addComponent<TransformComponent>(e3, HUGO::Vector3(20, -30, -50), HUGO::Quaternion(1,5,5,0), HUGO::Vector3(0.15, 0.25, 0.15));
    // scene->addComponent<RigidBodyComponent>(e3, shape, 5.f);
    // scene->addComponent<KinematicRBTag>(e3);
    // scene->addComponent<RenderComponent>(e3, "cube.mesh", "ColorVerde");
}

void PhysicsTestSystem::onLoad(){
    
}

void PhysicsTestSystem::fixedUpdate(double dt) {
    
}

float rate = 0.2f;
float t = 0;
void PhysicsTestSystem::update(double dt) {

}

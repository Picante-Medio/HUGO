#include "PhysicsTestSystem.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "TestComponent.h"

PhysicsTestSystem::~PhysicsTestSystem() {

}

void PhysicsTestSystem::init(){

    // Entidad 1: Cuerpo estático
    staticBase = scene.createEntity();

    scene.addComponent<TransformComponent>(staticBase, [](TransformComponent& transform){
        transform.position = {1.f, -3.f, 0.f};
        transform.scale = {1., 1.f, 1.f};
    });

    scene.addComponent<ColliderComponent>(staticBase, [](ColliderComponent& collider){
        collider.info = HUGO::BoxColliderInfo{HUGO::Vector3(5.f, 0.5f, 1.f)};
    });

    scene.addComponent<RigidBodyComponent>(staticBase, [](RigidBodyComponent& rb){
        rb.setBodyType(HUGO::BodyType::STATIC);
        // rb->setShape()
        // rb.setMass(5.f);
    });

    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene.createEntity();

    scene.addComponent<TransformComponent>(e2, [](TransformComponent& transform){
        transform.position = {0.f, 5, 0.f};
        transform.scale = {1.f, 1.f, 1.f};
    });
    //scene.addComponent<TransformChangedTag>(e2);

    scene.addComponent<ColliderComponent>(e2, [](ColliderComponent& collider){
        collider.info = HUGO::CylinderColliderInfo{HUGO::Vector3(1.f, 2.f, 2.f)};
        collider.setRotationOffset(HUGO::Quaternion(HUGO::Radian(HUGO::Degree(90)), HUGO::Vector3(0, 0, 1)));
    });

    scene.addComponent<RigidBodyComponent>(e2, [](RigidBodyComponent& rb){
        rb.setMass(5.f);
        rb.setBodyType(HUGO::BodyType::DYNAMIC);
        // rb->setShape()
    });

    scene.addComponent<TestComponent>(e2);

    EntityID e3 = scene.createEntity();
    scene.addComponent<TransformComponent>(e3,[](TransformComponent& transform){
        transform.position = {5.f, 5.f, 0.f};
    });
    
    scene.addComponent<ColliderComponent>(e3, [](ColliderComponent& col){
        col.info = HUGO::BoxColliderInfo{HUGO::Vector3(0.5f, 0.5f, 0.5f)};
    });

    scene.addComponent<RigidBodyComponent>(e3, [](RigidBodyComponent& rb){
        rb.setMass(5.f);
        rb.setGravity(HUGO::Vector3(0, -3, 0));
    });

    scene.addComponent<RenderComponent>(e2, [](RenderComponent& render){
        render.setMeshName("cube.mesh");
    });
}

void PhysicsTestSystem::onLoad(){
    
}

void PhysicsTestSystem::fixedUpdate(uint32_t) {
    
}

void PhysicsTestSystem::update(uint32_t) {

    for(auto collisionId : scene.getComponentConst<ColliderComponent>(staticBase).getCollisionInfo()){
        if(scene.hasComponent<TestComponent>(collisionId)){
            scene.modifyComponent<RigidBodyComponent>(collisionId, [](RigidBodyComponent& rbCmp){
                rbCmp.applyForce(HUGO::Vector3(0, 100, 0));
            });
        }
    }
}

#include "PhysicsTestSystem.h"
#include "Scene.h"
#include "PhysicsTags.h"
#include "HugoMath.h"
#include "RigidBodyComponent.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

PhysicsTestSystem::PhysicsTestSystem(std::shared_ptr<Scene> scene)
: System(scene)
{
    
    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();
    TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(e1);
    transform.position = HUGO::Vector3(0, -20, 0);
    transform.scale = HUGO::Vector3(1, 0.5f, 0.5f);
    ColliderComponent& collider = scene->addAndGetComponent<ColliderComponent>(e1);
    collider.info = BoxColliderInfo{HUGO::Vector3(2.f, 1.f, 1.f)};
    RigidBodyComponent& rb = scene->addAndGetComponent<RigidBodyComponent>(e1);
    // rb->setShape()
    scene->addComponent<StaticRBTag>(e1);
    // RenderComponent& rc = scene->addAndGetComponent<RenderComponent>(e1);
    // rc.setMeshName("cube.mesh");
    // rc.setMaterialName("prueba/ColorRosa");
    
    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();
    TransformComponent& transform2 = scene->addAndGetComponent<TransformComponent>(e2);
    transform2.position = HUGO::Vector3(0, 10, 0);
    transform2.scale = HUGO::Vector3(0.15f, 0.25f, 0.15f);
    ColliderComponent& collider2 = scene->addAndGetComponent<ColliderComponent>(e2);
    collider2.info = BoxColliderInfo{HUGO::Vector3(1.f, 1.f, 1.f)};
    RigidBodyComponent& rb2 = scene->addAndGetComponent<RigidBodyComponent>(e2);
    rb2.setMass(5.f);
    // rb->setShape()
    scene->addComponent<DynamicRBTag>(e2);
    // RenderComponent& rc2 = scene->addAndGetComponent<RenderComponent>(e2);
    // rc2.setMeshName("cube.mesh");
    
    // Entidad 3: Cuerpo kinemático
    // EntityID e3 = scene->createEntity();
    // scene->addComponent<TransformComponent>(e3, HUGO::Vector3(20, -30, -50), HUGO::Quaternion(1,5,5,0), HUGO::Vector3(0.15, 0.25, 0.15));
    // scene->addComponent<RigidBodyComponent>(e3, shape, 5.f);
    // scene->addComponent<KinematicRBTag>(e3);
    // scene->addComponent<RenderComponent>(e3, "cube.mesh", "ColorVerde");
}

PhysicsTestSystem::~PhysicsTestSystem() {

}

void PhysicsTestSystem::fixedUpdate(double dt) {
    for(auto pack : scene->query<TransformComponent, StaticRBTag>()) {
        TransformComponent& body = pack.get<TransformComponent&>();
    }

    for(auto pack : scene->query<TransformComponent, RigidBodyComponent, DynamicRBTag>()) {
        RigidBodyComponent& body = pack.get<RigidBodyComponent&>();
    }
}

void PhysicsTestSystem::update(double dt) {

}

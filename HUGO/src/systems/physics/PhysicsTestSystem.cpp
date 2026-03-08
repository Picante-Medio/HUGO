#include "PhysicsTestSystem.h"
#include "Scene.h"
#include "PhysicsTags.h"
#include "HugoMath.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

PhysicsTestSystem::PhysicsTestSystem(std::shared_ptr<Scene> scene)
: System(scene)
{
    BoxColliderInfo boxInfo{HUGO::Vector3(40,8,10)};
    // Shape shape{boxInfo, HUGO::INVALID_SHAPE_ID};
    Shape shape;
    shape.info = boxInfo;

    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();
    scene->addComponent<TransformComponent>(e1, HUGO::Vector3(0, -20, -50), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(1, 0.05, 0.5));
    scene->addComponent<RigidBodyComponent>(e1, shape);
    scene->addComponent<StaticRBTag>(e1);
    //scene->addComponent<RenderComponent>(e1, "cube.mesh", "ColorVerde");
    
    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2,  HUGO::Vector3(20, 30, -50), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.15, 0.25, 0.15));
    scene->addComponent<RigidBodyComponent>(e2, shape, 5.f);
    scene->addComponent<DynamicRBTag>(e2);
    scene->addComponent<RenderComponent>(e2, "cube.mesh");
    
    // // Entidad 3: Cuerpo kinemático
    // EntityID e3 = scene->createEntity();
    // scene->addComponent<TransformComponent>(e3, HUGO::Vector3(-30, 30, -50), HUGO::Quaternion(1,5,5,0), HUGO::Vector3(0.15, 0.25, 0.15));
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
    for(auto pack : scene->query<TransformComponent, RigidBodyComponent, DynamicRBTag>()) {
        TransformComponent& body = pack.get<TransformComponent&>();
    }
}

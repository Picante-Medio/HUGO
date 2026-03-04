#include "PhysicsTestSystem.h"
#include "Scene.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "PhysicsTags.h"
#include "HugoMath.h"

PhysicsTestSystem::PhysicsTestSystem(std::shared_ptr<Scene> scene)
: System(scene)
{
    BoxColliderInfo boxInfo{HUGO::Vector3(1,1,1)};
    // Shape shape{boxInfo, HUGO::INVALID_SHAPE_ID};
    Shape shape;
    shape.info = boxInfo;

    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();
    scene->addComponent<TransformComponent>(e1);
    scene->addComponent<RigidBodyComponent>(e1, shape);
    scene->addComponent<StaticRBTag>(e1);
    
    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2);
    scene->addComponent<RigidBodyComponent>(e2, shape);
    
    // Entidad 3: Cuerpo kinemático
    EntityID e3 = scene->createEntity();
    scene->addComponent<TransformComponent>(e3);
    scene->addComponent<RigidBodyComponent>(e3, shape);
    scene->addComponent<KinematicRBTag>(e3);


}

PhysicsTestSystem::~PhysicsTestSystem() {

}

void PhysicsTestSystem::fixedUpdate(double dt) {
    for(auto pack : scene->query<TransformComponent, KinematicRBTag>()) {
        TransformComponent& body = pack.get<TransformComponent&>();
        std::cout << body.position.x << '\n';
    }
}

void PhysicsTestSystem::update(double dt) {
}
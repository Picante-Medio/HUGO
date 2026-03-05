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
    BoxColliderInfo boxInfo{HUGO::Vector3(1,1,1)};
    // Shape shape{boxInfo, HUGO::INVALID_SHAPE_ID};
    Shape shape;
    shape.info = boxInfo;

    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();
    scene->addComponent<TransformComponent>(e1, HUGO::Vector3(0, 30, 0));
    scene->addComponent<RigidBodyComponent>(e1, shape);
    scene->addComponent<StaticRBTag>(e1);
    // scene->addComponent<RenderComponent>(e1, "cube.mesh");
    
    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2,  HUGO::Vector3(0, 30, -50), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.5, 0.5, 0.5));
    scene->addComponent<RigidBodyComponent>(e2, shape, 5.f);
    scene->addComponent<DynamicRBTag>(e2);
    scene->addComponent<RenderComponent>(e2, "cube.mesh");
    
    // Entidad 3: Cuerpo kinemático
    EntityID e3 = scene->createEntity();
    scene->addComponent<TransformComponent>(e3);
    scene->addComponent<RigidBodyComponent>(e3, shape, 5.f);
    scene->addComponent<KinematicRBTag>(e3);
}

PhysicsTestSystem::~PhysicsTestSystem() {

}

void PhysicsTestSystem::fixedUpdate(double dt) {
    for(auto pack : scene->query<TransformComponent, StaticRBTag>()) {
        TransformComponent& body = pack.get<TransformComponent&>();
    }

    for(auto pack : scene->query<TransformComponent, RigidBodyComponent, DynamicRBTag>()) {
        RigidBodyComponent& body = pack.get<RigidBodyComponent&>();

        body.applyForce(HUGO::Vector3(10,0,0));
        std::cout << "a\n";
    }
}

void PhysicsTestSystem::update(double dt) {
    for(auto pack : scene->query<TransformComponent, RigidBodyComponent, DynamicRBTag>()) {
        TransformComponent& body = pack.get<TransformComponent&>();

        std::cout << "Posicion del body: (" << body.position.x << " " << body.position.y << " " << body.position.z << ")\n"; 
    }
}
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
    Shape shape;
    shape.info = boxInfo;
    
    // Entidad 1: Cuerpo estático
    EntityID e1 = scene->createEntity();
    TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(e1);
    transform.position = HUGO::Vector3(20, -30, -50);
    transform.scale = HUGO::Vector3(1, 0.05f, 0.5f);
    RigidBodyComponent& rb = scene->addAndGetComponent<RigidBodyComponent>(e1);
    // rb->setShape()
    scene->addComponent<StaticRBTag>(e1);
    RenderComponent& rc = scene->addAndGetComponent<RenderComponent>(e1);
    rc.setMeshName("cube.mesh");
    rc.setMaterialName("prueba/ColorRose");
    
    // Entidad 2: Cuerpo dinámico
    EntityID e2 = scene->createEntity();
    transform = scene->addAndGetComponent<TransformComponent>(e2);
    transform.position = HUGO::Vector3(20, 30, -50);
    transform.scale = HUGO::Vector3(0.15f, 0.25f, 0.15f);
    rb = scene->addAndGetComponent<RigidBodyComponent>(e2);
    // rb->setShape()
    scene->addComponent<DynamicRBTag>(e2);
    rc = scene->addAndGetComponent<RenderComponent>(e2);
    rc.setMeshName("cube.mesh");
    
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
    for(auto pack : scene->query<TransformComponent, RigidBodyComponent, DynamicRBTag>()) {
        TransformComponent& tf = pack.get<TransformComponent&>();
        RigidBodyComponent& rb = pack.get<RigidBodyComponent&>();

        if(rb.getCollisionInfo().empty()){
            std::cout << "Posicion: " << tf.position.x << " " << tf.position.y << " " << tf.position.z << " No choca\n";
        }
        else if(rb.getCollisionInfo().size() > 1){
            std::cout << "La colisiones no se estan borrando bien";
        }
        else
        {
            std::cout << "Posicion: " << tf.position.x << " " << tf.position.y << " " << tf.position.z << " " << rb.entity << " choca con " << rb.getCollisionInfo().at(0) << "\n";
        }
    }
}

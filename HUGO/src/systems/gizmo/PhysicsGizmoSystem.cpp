#include "PhysicsGizmoSystem.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "HugoError.h"
#include "HugoPhysics.h"

void PhysicsGizmoSystem::init(){
    //scene->onAddComponent<LightComponent>().connect(this, &LightSystem::registerLightEntity);
    scene->onAddComponent<ColliderComponent>().connect(this, &PhysicsGizmoSystem::createGizmo);
    scene->onRemoveComponent<ColliderComponent>().connect(this, &PhysicsGizmoSystem::destroyGizmo);
}

void PhysicsGizmoSystem::onLoad(){

}

#pragma region Gizmo Handling

//---------------GIZMO MESH CREATION-----------------

//If updateGizmo has no specialization, then it is not supported
template <typename T>
ErrorOrVoid createGizmoMesh(std::shared_ptr<HUGO::IScene>, EntityID entity, const T& colliderInfo){
    return ErrorOrVoid(StringError{"Collider shape not contemplated by PhysicsGizmoSystem"});
}

template <>
ErrorOrVoid createGizmoMesh(std::shared_ptr<HUGO::IScene> scene, EntityID entity, const HUGO::BoxColliderInfo& colliderInfo) {

    if(scene->getComponent<RenderComponent>(entity).getMeshName() != "cube.mesh") {

        scene->modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
            gizmoRender.setMeshName("cube.mesh");
            gizmoRender.setMaterialName("debug/WireframeMaterial");
        });
    }

    scene->modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.halfExtents * 2);
    });

    return ErrorOrVoid();
}

template <>
ErrorOrVoid createGizmoMesh(std::shared_ptr<HUGO::IScene> scene, EntityID entity, const HUGO::SphereColliderInfo& colliderInfo) {

    if(scene->getComponent<RenderComponent>(entity).getMeshName() != "sphere.mesh") {

        scene->modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
            gizmoRender.setMeshName("sphere.mesh");
            gizmoRender.setMaterialName("debug/WireframeMaterial");
        });
    }

    scene->modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.r * 2);
    });

    return ErrorOrVoid();
}

template <>
ErrorOrVoid createGizmoMesh(std::shared_ptr<HUGO::IScene> scene, EntityID entity, const HUGO::CylinderColliderInfo& colliderInfo){

    if(scene->getComponent<RenderComponent>(entity).getMeshName() != "cylinder.mesh") {

        scene->modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
            gizmoRender.setMeshName("cylinder.mesh");
            gizmoRender.setMaterialName("debug/WireframeMaterial");
        });
    }

    scene->modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.halfExtents * 2);
    });

    return ErrorOrVoid();
}

template <>
ErrorOrVoid createGizmoMesh(std::shared_ptr<HUGO::IScene> scene, EntityID entity, const HUGO::ConeColliderInfo& colliderInfo){

    if(scene->getComponent<RenderComponent>(entity).getMeshName() != "cone.mesh") {

        scene->modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
            gizmoRender.setMeshName("cone.mesh");
            gizmoRender.setMaterialName("debug/WireframeMaterial");
        });
    }

    scene->modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2));
    });

    return ErrorOrVoid();
}

template <>
ErrorOrVoid createGizmoMesh<HUGO::CapsuleColliderInfo>(std::shared_ptr<HUGO::IScene> scene, EntityID entity, const HUGO::CapsuleColliderInfo& colliderInfo){

    if(scene->getComponent<RenderComponent>(entity).getMeshName() != "capsule.mesh") {

        scene->modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
            gizmoRender.setMeshName("capsule.mesh");
            gizmoRender.setMaterialName("debug/WireframeMaterial");
        });
    }

    scene->modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2));
    });

    return ErrorOrVoid();
}

//------------------GIZMO CREATION------------------------

//Creates gizmo with transform and shape mesh
void PhysicsGizmoSystem::createGizmo(ColliderComponent& collider){
    EntityID newID = scene->createEntity();

    scene->addComponent<TransformComponent>(newID);
    scene->addComponent<RenderComponent>(newID);

    //C++ magic to determine the type that holds collider.info without if-else madness
    ErrorOrVoid res = std::visit([&](auto&& info) {return createGizmoMesh(scene, newID, info);}, collider.info);

    if(res.isError()) {
        scene->log(std::make_shared<StringError>("[PhysicsGizmoSystem] Error creating Gizmo: "+ res.getError().errorMessage));
        return;
    }

    m_gizmoEntities[collider.entity] = newID;
}

void PhysicsGizmoSystem::destroyGizmo(ColliderComponent& coll){
    scene->removeEntity(m_gizmoEntities[coll.entity]);
    m_gizmoEntities.erase(coll.entity);
}

#pragma endregion

void PhysicsGizmoSystem::update(double){
#ifndef NDEBUG
    
    for(auto pack : scene->query<ColliderComponent, TransformComponent>()) {
        const ColliderComponent& collider = pack.getConst<ColliderComponent>();
        const TransformComponent& transform = pack.getConst<TransformComponent>();

        //If we dont want gizmos, we skip all of this
        if(!collider.useGizmo) continue;

        //Update transform position and rotation based on the colliders transform
        EntityID gizmoID= m_gizmoEntities[transform.entity];

        scene->modifyComponent<TransformComponent>(gizmoID, [&transform](TransformComponent& gizmoTr){
            gizmoTr.setPosition(transform.getPosition());
            gizmoTr.setRotation(transform.getRotation());
        });
    }

#endif
}



void PhysicsGizmoSystem::fixedUpdate(double) {}

#include "PhysicsGizmoSystem.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "HugoError.h"
#include "HugoPhysics.h"

void PhysicsGizmoSystem::init(){

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
ErrorOrType<EntityID> createGizmoEntity(std::shared_ptr<HUGO::IScene> scene, const ColliderComponent& collider){
    EntityID newID = scene->createEntity();

    scene->addComponent<TransformComponent>(newID);
    scene->addComponent<RenderComponent>(newID);

    //C++ magic to determine the type that holds collider.info without if-else madness
    ErrorOrVoid res = std::visit([&](auto&& info) {return createGizmoMesh(scene, newID, info);}, collider.info);

    if(res.isError()) return ErrorOrType<EntityID>(StringError{"[PhysicsGizmoSystem] Error creating Gizmo: "+ res.getError().errorMessage});

    return ErrorOrType<EntityID>(newID);
}

#pragma endregion

void PhysicsGizmoSystem::update(double){
#ifndef NDEBUG
    
    for(auto pack : scene->query<ColliderComponent, TransformComponent>()) {
        const ColliderComponent& collider = pack.getConst<ColliderComponent>();
        const TransformComponent& transform = pack.getConst<TransformComponent>();

        //If we dont want gizmos, we skip all of this
        if(!collider.useGizmo) continue;

        //If collider isnt registered, we register it in the map and create a gizmo entity
        if(m_gizmoEntities.find(transform.entity) == m_gizmoEntities.end()) {
            ErrorOrType<EntityID> newID = createGizmoEntity(scene, collider);

            //If an error occured when creating the gizmo entity, current case is skipped
            if(newID.isError()) {
                auto err = std::shared_ptr<StringError>();
                err->errorMessage = newID.getError().errorMessage;
                scene->log(std::shared_ptr<Error>(err));
                continue;
            }
            m_gizmoEntities[transform.entity] = {newID.getValue(), false};
        }

        //Update transform position and rotation based on the colliders transform

        EntityVisited& entVisited = m_gizmoEntities[transform.entity];
        entVisited.visited = true;

        scene->modifyComponent<TransformComponent>(entVisited.entityID, [&transform](TransformComponent& gizmoTr){
            gizmoTr.setPosition(transform.getPosition());
            gizmoTr.setRotation(transform.getRotation());
        });
    }

    //If an entry was not visited, we remove that entry.
    for(auto it = m_gizmoEntities.begin(); it != m_gizmoEntities.end();){
        if(!it->second.visited) 
        {
            it = m_gizmoEntities.erase(it);
        }
        else {
            it->second.visited = false;
            it++; 
        }
    }

#endif
}



void PhysicsGizmoSystem::fixedUpdate(double) {}

#include "PhysicsGizmoSystem.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "HugoError.h"
#include "RenderComponent.h"
#include "RenderTags.h"

void PhysicsGizmoSystem::init(){

}

void PhysicsGizmoSystem::onLoad(){

}

#pragma region Gizmo Handling

//---------------GIZMO UPDATE-----------------

//If updateGizmo has no specialization, then it is not supported
template <typename T>
ErrorOrVoid updateGizmo(std::shared_ptr<HUGO::IScene>,TransformComponent&, RenderComponent&, T){
    return ErrorOrVoid(StringError{"Collider shape not contemplated by PhysicsGizmoSystem"});
}

template <>
ErrorOrVoid updateGizmo<BoxColliderInfo>(std::shared_ptr<HUGO::IScene> scene, TransformComponent& gizmoTransform, RenderComponent& gizmoRender, BoxColliderInfo colliderInfo){
    if(gizmoRender.getMeshName() != "cube.mesh"){
        gizmoRender.setMeshName("cube.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
        scene->addAndGetComponent<MaterialDirtyTag>(gizmoRender.entity);
    }
    gizmoTransform.scale = colliderInfo.halfExtents * 2;

    return ErrorOrVoid();
}

template <>
ErrorOrVoid updateGizmo<SphereColliderInfo>(std::shared_ptr<HUGO::IScene> scene, TransformComponent& gizmoTransform, RenderComponent& gizmoRender, SphereColliderInfo colliderInfo){
    if(gizmoRender.getMeshName() != "sphere.mesh"){
        gizmoRender.setMeshName("sphere.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
        scene->addAndGetComponent<MaterialDirtyTag>(gizmoRender.entity);
    }
    gizmoTransform.scale = HUGO::Vector3(colliderInfo.r * 2);

    return ErrorOrVoid();
}

template <>
ErrorOrVoid updateGizmo<CylinderColliderInfo>(std::shared_ptr<HUGO::IScene> scene, TransformComponent& gizmoTransform, RenderComponent& gizmoRender, CylinderColliderInfo colliderInfo){
    if(gizmoRender.getMeshName() != "cylinder.mesh"){
        gizmoRender.setMeshName("cylinder.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
        scene->addAndGetComponent<MaterialDirtyTag>(gizmoRender.entity);
    }
    gizmoTransform.scale = colliderInfo.halfExtents * 2;

    return ErrorOrVoid();
}

template <>
ErrorOrVoid updateGizmo<ConeColliderInfo>(std::shared_ptr<HUGO::IScene> scene, TransformComponent& gizmoTransform, RenderComponent& gizmoRender, ConeColliderInfo colliderInfo){
    if(gizmoRender.getMeshName() != "cone.mesh"){
        gizmoRender.setMeshName("cone.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
        scene->addAndGetComponent<MaterialDirtyTag>(gizmoRender.entity);
    }
    gizmoTransform.scale = HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2);

    return ErrorOrVoid();
}

template <>
ErrorOrVoid updateGizmo<CapsuleColliderInfo>(std::shared_ptr<HUGO::IScene> scene, TransformComponent& gizmoTransform, RenderComponent& gizmoRender, CapsuleColliderInfo colliderInfo){
    if(gizmoRender.getMeshName() != "capsule.mesh"){
        gizmoRender.setMeshName("capsule.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
        scene->addAndGetComponent<MaterialDirtyTag>(gizmoRender.entity);
    }
    gizmoTransform.worldScale = HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2);

    return ErrorOrVoid();
}

//------------------GIZMO CREATION------------------------
ErrorOrType<EntityID> createGizmo(std::shared_ptr<HUGO::IScene> scene, ColliderComponent& collider, TransformComponent& colliderTransform){
    EntityID newID = scene->createEntity();
    TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
    RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);

    return ErrorOrType<EntityID>(newID);
}

#pragma endregion

void PhysicsGizmoSystem::update(double){
#ifndef NDEBUG
    
    for(auto pack : scene->query<ColliderComponent, TransformComponent>()){
        ColliderComponent& collider = pack.getDeprecated<ColliderComponent&>();
        TransformComponent& transform = pack.getDeprecated<TransformComponent&>();

        //If we dont want gizmos, we skip all of this
        if(!collider.useGizmo) continue;

        //If collider isnt registered, we register it in the map and create a gizmo entity
        if(m_gizmoEntities.find(transform.entity) == m_gizmoEntities.end()) {
            ErrorOrType<EntityID> newID = createGizmo(scene, collider, transform);

            //If an error occured when creating the gizmo entity, current case is skipped
            if(newID.isError()) continue;
            m_gizmoEntities[transform.entity] = {newID.getValue(), false};
        }

        //Update transform position and rotation based on the colliders transform

        EntityVisited& entVisited = m_gizmoEntities[transform.entity];
        entVisited.visited = true;

        TransformComponent& gizmoTr = scene->getComponent<TransformComponent>(entVisited.entityID);
        RenderComponent& gizmoRender = scene->getComponent<RenderComponent>(entVisited.entityID);

        gizmoTr.position = transform.position;
        gizmoTr.rotation = transform.rotation;

        //C++ magic to determine the type that holds collider.info without if-else madness
        std::visit([&](auto&& info) {updateGizmo(scene, gizmoTr, gizmoRender, info);}, collider.info);
    }

    //If an entry was not visited, we remove that entry.
    for(auto it = m_gizmoEntities.begin(); it != m_gizmoEntities.end();){
        if(!it->second.visited) m_gizmoEntities.erase(it);
        else {
            it->second.visited = false;
            it++; 
        }
    }

#endif
}



void PhysicsGizmoSystem::fixedUpdate(double) {}

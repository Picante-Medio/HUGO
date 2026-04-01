#include "PhysicsGizmoSystem.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "HugoError.h"
#include "RenderComponent.h"

void PhysicsGizmoSystem::init(){

}

void PhysicsGizmoSystem::onLoad(){

}

#pragma region Gizmo Handling
ErrorOrType<EntityID> createGizmo(std::shared_ptr<HUGO::IScene> scene, ColliderComponent& collider, TransformComponent& colliderTransform){
    if(std::holds_alternative<BoxColliderInfo>(collider.info)){

        EntityID newID = scene->createEntity();
        TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
        RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);
        gizmoRender.setMeshName("cube.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");

        BoxColliderInfo collInfo = std::get<BoxColliderInfo>(collider.info);
        gizmoTr.scale = collInfo.halfExtents * 2;

        return ErrorOrType<EntityID>(newID);
    }
    else if (std::holds_alternative<SphereColliderInfo>(collider.info)){
        EntityID newID = scene->createEntity();
        TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
        RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);
        gizmoRender.setMeshName("sphere.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");

        SphereColliderInfo collInfo = std::get<SphereColliderInfo>(collider.info);
        gizmoTr.scale = HUGO::Vector3(collInfo.r * 2);

        return ErrorOrType<EntityID>(newID);
    }
    else if (std::holds_alternative<CylinderColliderInfo>(collider.info)){
        EntityID newID = scene->createEntity();
        TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
        RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);
        gizmoRender.setMeshName("cylinder.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");

        CylinderColliderInfo collInfo = std::get<CylinderColliderInfo>(collider.info);
        gizmoTr.scale = collInfo.halfExtents * 2;

        return ErrorOrType<EntityID>(newID);
    }
    else if (std::holds_alternative<ConeColliderInfo>(collider.info)){
        EntityID newID = scene->createEntity();
        TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
        RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);
        gizmoRender.setMeshName("cone.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");

        ConeColliderInfo collInfo = std::get<ConeColliderInfo>(collider.info);
        gizmoTr.scale = HUGO::Vector3(collInfo.r * 2, collInfo.height, collInfo.r * 2);

        return ErrorOrType<EntityID>(newID);
    }
    else if (std::holds_alternative<CapsuleColliderInfo>(collider.info)){
        EntityID newID = scene->createEntity();
        TransformComponent& gizmoTr = scene->addAndGetComponent<TransformComponent>(newID);
        RenderComponent& gizmoRender = scene->addAndGetComponent<RenderComponent>(newID);
        gizmoRender.setMeshName("capsule.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");

        CapsuleColliderInfo collInfo = std::get<CapsuleColliderInfo>(collider.info);
        gizmoTr.worldScale = HUGO::Vector3(collInfo.r * 2, collInfo.height, collInfo.r * 2);

        return ErrorOrType<EntityID>(newID);
    }
    
    return ErrorOrType<EntityID>(StringError{"Collider shape not contemplated by PhysicsGizmoSystem"});
}

#pragma endregion

void PhysicsGizmoSystem::update(double dt){
#ifndef NDEBUG
    
    for(auto pack : scene->query<ColliderComponent, TransformComponent>()){
        ColliderComponent& collider = pack.getConst<ColliderComponent&>();
        TransformComponent& transform = pack.getConst<TransformComponent&>();

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

        gizmoTr.position = transform.position;
        gizmoTr.position = transform.position;
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



void PhysicsGizmoSystem::fixedUpdate(double dt){
    
}
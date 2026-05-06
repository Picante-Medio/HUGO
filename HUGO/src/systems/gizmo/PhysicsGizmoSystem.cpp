#include "PhysicsGizmoSystem.h"
#include "ColliderComponent.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "HugoError.h"
#include "HugoPhysics.h"

void PhysicsGizmoSystem::init(){
    scene.onAddComponent<ColliderComponent>().connect(this, &PhysicsGizmoSystem::createGizmo);
    scene.onRemoveComponent<ColliderComponent>().connect(this, &PhysicsGizmoSystem::destroyGizmo);

    scene.onUpdateComponent<ColliderComponent>().connect(this, &PhysicsGizmoSystem::onUpdateGizmo);
}

void PhysicsGizmoSystem::onLoad(){

}

#pragma region Gizmo Handling

//---------------GIZMO MESH CREATION-----------------

//If updateGizmo has no specialization, then it is not supported
template <typename T>
HUGO::ErrorOrVoid createGizmoMesh(HUGO::IScene&, EntityID entity, const T& colliderInfo){
    return HUGO::ErrorOrVoid(HUGO::StringError{"Collider shape not contemplated by PhysicsGizmoSystem"});
}

template <>
HUGO::ErrorOrVoid createGizmoMesh(HUGO::IScene& scene, EntityID entity, const HUGO::BoxColliderInfo& colliderInfo) {
    scene.modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
        gizmoRender.setMeshName("cube.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
    });

    scene.modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.halfExtents * 2);
    });

    return HUGO::ErrorOrVoid();
}

template <>
HUGO::ErrorOrVoid createGizmoMesh(HUGO::IScene& scene, EntityID entity, const HUGO::SphereColliderInfo& colliderInfo) {

    scene.modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
        gizmoRender.setMeshName("sphere.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
    });

    scene.modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.r * 2);
    });

    return HUGO::ErrorOrVoid();
}

template <>
HUGO::ErrorOrVoid createGizmoMesh(HUGO::IScene& scene, EntityID entity, const HUGO::CylinderColliderInfo& colliderInfo){

    scene.modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
        gizmoRender.setMeshName("cylinder.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
    });

    scene.modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(colliderInfo.halfExtents * 2);
    });

    return HUGO::ErrorOrVoid();
}

template <>
HUGO::ErrorOrVoid createGizmoMesh(HUGO::IScene& scene, EntityID entity, const HUGO::ConeColliderInfo& colliderInfo){

    scene.modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
        gizmoRender.setMeshName("cone.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
    });

    scene.modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2));
    });

    return HUGO::ErrorOrVoid();
}

template <>
HUGO::ErrorOrVoid createGizmoMesh<HUGO::CapsuleColliderInfo>(HUGO::IScene& scene, EntityID entity, const HUGO::CapsuleColliderInfo& colliderInfo){

    scene.modifyComponent<RenderComponent>(entity, [](RenderComponent& gizmoRender) {
        gizmoRender.setMeshName("capsule.mesh");
        gizmoRender.setMaterialName("debug/WireframeMaterial");
    });

    scene.modifyComponent<TransformComponent>(entity, [&colliderInfo](TransformComponent& gizmoTransform){
        gizmoTransform.setScale(HUGO::Vector3(colliderInfo.r * 2, colliderInfo.height, colliderInfo.r * 2));
    });

    return HUGO::ErrorOrVoid();
}

//------------------GIZMO CREATION------------------------

//Creates gizmo with transform and shape mesh
void PhysicsGizmoSystem::createGizmo(ColliderComponent& collider){

    if(!collider.useGizmo) return;

    EntityID newID = scene.createEntity();

    scene.addComponent<TransformComponent>(newID);
    scene.addComponent<RenderComponent>(newID);

    //C++ magic to determine the type that holds collider.info without if-else madness
    HUGO::ErrorOrVoid res = std::visit([&](auto&& info) {return createGizmoMesh(scene, newID, info);}, collider.info);

    if(res.isError()) {
        scene.log(std::make_shared<HUGO::StringError>("[PhysicsGizmoSystem] Error creating Gizmo: "+ res.getError().errorMessage));
        return;
    }

    m_gizmoEntities[collider.entity] = newID;
}

void PhysicsGizmoSystem::destroyGizmo(ColliderComponent& coll){

    if(m_gizmoEntities.find(coll.entity) == m_gizmoEntities.end()) return;

    scene.removeEntity(m_gizmoEntities[coll.entity]);
    m_gizmoEntities.erase(coll.entity);
}

void PhysicsGizmoSystem::onUpdateGizmo(ColliderComponent& coll){

    bool gizmoRegistered = m_gizmoEntities.find(coll.entity) != m_gizmoEntities.end();

    if(coll.useGizmo && !gizmoRegistered){
        createGizmo(coll);
    }
    
    if(!coll.useGizmo && gizmoRegistered){
        destroyGizmo(coll);
    }
}

#pragma endregion

void PhysicsGizmoSystem::update(uint32_t){
#ifndef NDEBUG
    
    for(auto pack : scene.query<ColliderComponent, TransformComponent>()) {
        const ColliderComponent& collider = pack.getConst<ColliderComponent>();
        const TransformComponent& transform = pack.getConst<TransformComponent>();

        //If we dont want gizmos, we skip all of this
        if(!collider.useGizmo) continue;


        //Update transform position and rotation based on the colliders transform
        EntityID gizmoID= m_gizmoEntities[transform.entity];

        scene.modifyComponent<TransformComponent>(gizmoID, [&transform, &collider](TransformComponent& gizmoTr){
            gizmoTr.setPosition(transform.getWorldPosition() + collider.getPositionOffset());
            gizmoTr.setRotation(transform.getWorldRotation() * collider.getRotationOffset());
        });
    }

#endif
}



void PhysicsGizmoSystem::fixedUpdate(uint32_t) {}

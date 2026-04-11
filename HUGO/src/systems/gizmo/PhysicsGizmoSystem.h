#pragma once
#include "HugoSystemMacros.h"
#include <unordered_map>
#include "EcsTypes.h"
#include "ColliderComponent.h"

HSYSTEM(PhysicsGizmoSystem)

public:
    void init();

    void onLoad();

    void update(double dt);

    void fixedUpdate(double dt);

    void createGizmo(ColliderComponent& coll);
    void destroyGizmo(ColliderComponent& coll);
private:
#ifndef NDEBUG

    /*
    First: Entity with collider
    Second: Gizmo entity associated
    */
    std::unordered_map<EntityID, EntityID> m_gizmoEntities;

#endif

END_HSYSTEM()


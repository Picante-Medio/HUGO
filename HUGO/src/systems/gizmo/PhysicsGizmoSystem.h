#pragma once
#include "HugoSystemMacros.h"
#include <unordered_map>
#include "EcsTypes.h"

HSYSTEM(PhysicsGizmoSystem)

public:
    void init();

    void onLoad();

    void update(double dt);

    void fixedUpdate(double dt);
private:
    struct EntityVisited{
        EntityID entityID;
        bool visited;
    };
#ifndef NDEBUG
    /*
        Key: Entity with collider
        Return: Gizmo entity
    */
    std::unordered_map<EntityID, EntityVisited> m_gizmoEntities;

#endif

END_HSYSTEM()


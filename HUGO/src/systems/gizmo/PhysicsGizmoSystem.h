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
    /*
        @param entityID Gizmo entity associated with physics collider
        @param visited Wether or not this collider was visited this frame
    */
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


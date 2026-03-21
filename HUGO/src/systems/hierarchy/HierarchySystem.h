#pragma once
#include "HugoError.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"
#include <unordered_set>

struct TransformComponent;
struct ParentComponent;

HSYSTEM(HierarchySystem)
private:

    void addChildToParent(EntityID child, EntityID parent);
    void removeChildFromParent(EntityID child, EntityID parent);
    bool hasCycle(EntityID child, EntityID newParent);
    void updateHierarchy();
    
    void addWorldDirtyTagForChildren(EntityID parent);

    void setLocalTransform(TransformComponent& transform);
    void updateWorldTransform(TransformComponent& transform, std::unordered_set<EntityID>& visited);
    TransformComponent& getFullTransform(EntityID entity, std::unordered_set<EntityID>& visited);
public:
    virtual ~HierarchySystem(){};

    void init();
    void onLoad();

    void update(double) override;

    void fixedUpdate(double) override;

END_HSYSTEM()
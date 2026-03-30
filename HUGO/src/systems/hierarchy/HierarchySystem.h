#pragma once
#include "HugoError.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"
#include <unordered_set>

struct TransformComponent;
struct ParentComponent;

HSYSTEM(HierarchySystem)
private:
    //functions to update hierarchy
    void addParentToChild(EntityID child, EntityID parent);
    void removeParentFromChild(EntityID child);
    void addChildToParent(EntityID child, EntityID parent);
    void removeChildFromParent(EntityID child, EntityID parent);
    bool hasCycle(EntityID child, EntityID newParent);
    void updateHierarchy();
    
    //functions to update transform
    void addWorldDirtyTagForChildren(EntityID parent);
    void setLocalTransform(TransformComponent& transform);
    void updateWorldTransform(TransformComponent& transform);
    TransformComponent& getFullTransform(EntityID entity);
public:
    virtual ~HierarchySystem(){};

    void init();
    void onLoad();

    void update(double) override;

    void fixedUpdate(double) override;

END_HSYSTEM()
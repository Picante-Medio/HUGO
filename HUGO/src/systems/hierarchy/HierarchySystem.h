#pragma once
#include "HugoError.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"

struct TransformComponent;
struct ParentComponent;
struct ChildrenComponent;

HSYSTEM(HierarchySystem)
private:
    //functions to update hierarchy
    void addParentToChild(EntityID child, EntityID parent);
    void removeParentFromChild(EntityID child);
    void addChildToParent(EntityID child, EntityID parent);
    void removeChildFromParent(EntityID child, EntityID parent);
    bool hasCycle(EntityID child, EntityID newParent);

    void onAddTransformComponent(TransformComponent& tr);
    
    void onUpdateChildrenComponent(ChildrenComponent& childrenComp);
    void onUpdateParentComponent(ParentComponent& parentComp);

    void onRemoveChildrenComponent(ChildrenComponent& childrenComp);
    void onRemoveParentComponent(ParentComponent& parentComp);
    
    //functions to update transform
    //world-->local
    void setWorldTransform(TransformComponent& transform);
    void updateLocalTransform(TransformComponent& transform);
    //local --> world
    void setLocalTransform(TransformComponent& transform);
    void updateWorldTransform(TransformComponent& transform);
    TransformComponent& getFullTransform(EntityID entity);
    void propagateNeedUpdateToChildren(EntityID parent);
public:
    virtual ~HierarchySystem(){};

    void init();
    void onLoad();

    void update(double) override;

    void fixedUpdate(double) override;

END_HSYSTEM()
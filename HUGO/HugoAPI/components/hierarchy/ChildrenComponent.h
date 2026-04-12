#pragma once
#include "HugoComponentMacros.h"
#include <unordered_set>
#include <queue>
HCOMPONENT(ChildrenComponent)
private:
    std::unordered_set<EntityID> children;
    std::queue<EntityID> addedChildren;
    std::queue<EntityID> removedChildren;
public:
    inline void addChild(EntityID child){
        children.insert(child);
        addedChildren.push(child);
    }
    inline void removeChild(EntityID child){
        children.erase(child);
        removedChildren.push(child);
    }
    inline const std::unordered_set<EntityID>& getChildren() const{
        return children;
    }
    friend class HierarchySystem;
END_HCOMPONENT()


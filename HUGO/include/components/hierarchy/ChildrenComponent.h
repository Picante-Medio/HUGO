#pragma once
#include "HugoComponentMacros.h"

HCOMPONENT(ChildrenComponent)
private:
    std::vector<EntityID> children;
    std::vector<EntityID> addedChildren;
    std::vector<EntityID> removedChildren;
public:
    inline void addChild(EntityID child){
        addedChildren.push_back(child);
    }
    inline void removeChild(EntityID child){
        removedChildren.push_back(child);
    }
    friend class HierarchySystem;
END_HCOMPONENT()
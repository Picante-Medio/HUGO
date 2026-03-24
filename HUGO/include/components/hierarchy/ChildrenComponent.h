#pragma once
#include "HugoComponentMacros.h"

HCOMPONENT(ChildrenComponent)
private:
    std::vector<EntityID> children;
    std::vector<EntityID> addedChildren;
    std::vector<EntityID> removedChildren;
public:
    void addChild(EntityID entity){
        addedChildren.push_back(entity);
    }
    void removeChild(EntityID entity){
        removedChildren.push_back(entity);
    }
    friend class HierarchySystem;
END_HCOMPONENT()
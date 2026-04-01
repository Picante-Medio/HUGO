#pragma once
#include "HugoComponentMacros.h"
#include <unordered_set>
HCOMPONENT(ChildrenComponent)
private:
    std::unordered_set<EntityID> children;
    std::vector<EntityID> addedChildren;
    std::vector<EntityID> removedChildren;
public:
    inline void addChild(EntityID child){
        children.insert(child);
        addedChildren.push_back(child);
    }
    inline void removeChild(EntityID child){
        children.erase(child);
        removedChildren.push_back(child);
    }
    inline const std::unordered_set<EntityID>& getChildren() const{
        return children;
    }
    friend class HierarchySystem;
END_HCOMPONENT()
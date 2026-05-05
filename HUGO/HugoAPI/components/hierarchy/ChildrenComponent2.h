#include "HugoComponentMacros.h"
#include <unordered_set>
#include <queue>

HCOMPONENT(ChildrenComponent2)

std::unordered_set<EntityID> children;
std::queue<EntityID> removedChildren;
std::queue<EntityID> 

inline void addChild(EntityID child) {
    children.insert(child);
}

inline void removeChild(EntityID child) {
    children.erase(child);
}

END_HCOMPONENT()
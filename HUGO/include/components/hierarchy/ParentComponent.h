#pragma once
#include "HugoComponentMacros.h"
#include <optional>
HCOMPONENT(ParentComponent)
private:
    std::optional<EntityID> parent;
    std::optional<EntityID>  previousParent;
public:
    void setParent(EntityID newParent){
        parent = newParent;
    }
    std::optional<EntityID> getParent() const{
        return parent;
    }
    friend class HierarchySystem;
END_HCOMPONENT()


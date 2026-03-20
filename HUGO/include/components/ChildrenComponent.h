#pragma once
#include "HugoComponentMacros.h"

HCOMPONENT(ChildrenComponent)
    std::vector<EntityID> children;
END_HCOMPONENT()
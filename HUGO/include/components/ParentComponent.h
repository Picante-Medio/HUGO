#pragma once
#include "HugoComponentMacros.h"

HCOMPONENT(ParentComponent)
    EntityID parent = INVALID_ENTITY_ID;
    EntityID previousParent = INVALID_ENTITY_ID;
END_HCOMPONENT()

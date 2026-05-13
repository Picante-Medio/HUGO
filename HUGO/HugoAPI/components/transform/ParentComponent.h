#pragma once

#include "HUGO.h"
#include "HugoError.h"
#include <vector>

HCOMPONENT(ParentComponent)
public:
    EntityID parent = HUGO::INVALID_ENTITY;
END_HCOMPONENT()
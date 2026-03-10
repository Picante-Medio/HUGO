#pragma once

#include "Component.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(VisibilityDirtyTag)
END_HCOMPONENT()

HCOMPONENT(CanvasDirtyTag)
    HUGO::CanvasDirtyFlags flags;

    CanvasDirtyTag(EntityID entity,
                  HUGO::CanvasDirtyFlags flags)
        : Component(entity), flags(flags) {} 
END_HCOMPONENT()

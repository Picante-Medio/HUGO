#pragma once

#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(VisibilityDirtyTag)
END_HCOMPONENT()

HCOMPONENT(CanvasDirtyTag)
    HUGO::CanvasDirtyFlags flags = HUGO::CanvasDirtyFlags::NONE_CANVAS;
END_HCOMPONENT()

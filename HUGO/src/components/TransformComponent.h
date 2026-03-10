#pragma once

#include "Component.h"
#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(TransformComponent)

    HUGO::Vector3 position = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion rotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 scale = HUGO::Vector3(1, 1, 1);

END_HCOMPONENT()

HCOMPONENT(TransformDirtyTag)
END_HCOMPONENT()

#pragma once

#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(TransformComponent)
public:
    //Local transform
    HUGO::Vector3 position = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion rotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 scale = HUGO::Vector3(1, 1, 1);

    //world transform
    HUGO::Vector3 worldPosition = HUGO::Vector3(0, 0, 0);
    HUGO::Quaternion worldRotation = HUGO::Quaternion(1,0,0,0);
    HUGO::Vector3 worldScale = HUGO::Vector3(1, 1, 1);
END_HCOMPONENT()

HCOMPONENT(TransformDirtyTag)
END_HCOMPONENT()

HCOMPONENT(TransformChangeTag)
END_HCOMPONENT()

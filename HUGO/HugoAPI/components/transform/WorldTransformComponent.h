#pragma once

#include "HUGO.h"
#include <vector>

// If using default 
HCOMPONENT(WorldTransformComponent)
public:
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, worldPosition ,HUGO::Vector3());
    DEFAULT_SERIALIZED_MEMBER(HUGO::Quaternion, worldRotation, HUGO::Quaternion());
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, worldScale, HUGO::Vector3(1.0f));
END_HCOMPONENT()
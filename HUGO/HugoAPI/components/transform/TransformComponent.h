#pragma once

#include "HUGO.h"

HCOMPONENT(TransformComponent)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, position ,HUGO::Vector3());
    DEFAULT_SERIALIZED_MEMBER(HUGO::Quaternion, rotation, HUGO::Quaternion());
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, scale, HUGO::Vector3(1.0f));
END_HCOMPONENT()
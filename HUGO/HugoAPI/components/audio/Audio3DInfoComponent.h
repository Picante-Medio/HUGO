#pragma once

#include "HUGO.h"

// 3D SFX component
HCOMPONENT(Audio3DInfoComponent)
    // 3D attenuation distances
    DEFAULT_SERIALIZED_MEMBER(float, minDistance, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(float, maxDistance, 100.0f)
    // rolloff
    DEFAULT_SERIALIZED_MEMBER(float, rolloff, 2.0f) // how volume decreases with distance
END_HCOMPONENT()


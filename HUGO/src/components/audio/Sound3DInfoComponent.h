#pragma once
#include "Component.h"
#include "HugoTypes.h"

// 3D SFX component
struct Sound3DInfoComponent: public Component
{
    float minDistance;
    float maxDistance;
};


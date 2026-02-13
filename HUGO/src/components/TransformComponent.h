#pragma once

#include "Component.h"
#include "HugoMath.h"

struct TransformComponent: public Component
{
    HUGO::Vector3 position;
    HUGO::Quaternion rotation;
    HUGO::Vector3 scale;

    TransformComponent(EntityID entity,
                        const HUGO::Vector3& position = HUGO::Vector3(0, 0, 0),
                        const HUGO::Quaternion& rotation= HUGO::Quaternion(1,0,0,0),
                        const HUGO::Vector3& scale = HUGO::Vector3(1, 1, 1)): 
                        Component(entity), position(position), rotation(rotation), scale(scale) {}
};
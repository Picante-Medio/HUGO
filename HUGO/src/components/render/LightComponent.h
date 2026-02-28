#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

// Camera component that holds all necessary information for rendering a camera in the scene
struct LightComponent: public Component
{
    HUGO::Vector4 diffuse;
    HUGO::Vector4 specular;
    HUGO::LightType lightType = HUGO::LightType::LT_POINT;

    // Radian spotOuter;
    // Radian spotInner;
    // Real spotFalloff;
    // Real spotNearClip;

    // Vector4f attenuation;
    // Real shadowFarDist;
    // Real shadowFarDistSquared;
    // size_t indexInFrame;

    // Real shadowNearClipDist;
    // Real shadowFarClipDist;

    LightComponent(EntityID entity,
                   HUGO::Vector4 diff, HUGO::Vector4 specular, HUGO::LightType lType)
        : Component(entity), diffuse(diff), specular(specular), lightType(lType) {}
};
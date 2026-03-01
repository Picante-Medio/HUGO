#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

struct LightComponent: public Component
{
    HUGO::Vector4 diffuse = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    HUGO::Vector4 specular = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    HUGO::LightType lightType = HUGO::LightType::LT_POINT;

    HUGO::Radian spotOuter = HUGO::Radian(0.7f);
    HUGO::Radian spotInner = HUGO::Radian(0.6f);
    float spotFalloff = 1.0f;
    float spotNearClip = 1.0f;

    // Ogre explanition, from the documentation reference:
    // range - The absolute upper range of the light in world units.
    // constant	- The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation.
    // linear - The linear factor in the attenuation formula: 1 means attenuate evenly over the distance.
    // quadratic - The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
    HUGO::Vector4 attenuation = HUGO::Vector4(1.0f, 0.0f, 0.0f, 10000.0f); //range, constant, linear, quadratic
    float shadowFarDist;
    float shadowFarDistSquared;
    size_t indexInFrame;

    float shadowNearClipDist;
    float shadowFarClipDist;

    LightComponent(EntityID entity,
                   HUGO::Vector4 diff, HUGO::Vector4 specular, HUGO::LightType lType
                   , const HUGO::Radian& outer, const HUGO::Radian& inner
                   , float falloff, float nearClip, float shadowNearClipDist, float shadowFarClipDist)
        : Component(entity), diffuse(diff), specular(specular), lightType(lType), spotOuter(outer)
        , spotInner(inner), spotFalloff(falloff), spotNearClip(nearClip)
        , shadowNearClipDist(shadowNearClipDist), shadowFarClipDist(shadowFarClipDist) {}
};
#pragma once

#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(LightComponent)

private:
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
    float shadowFarDist = 1.0f;
    float shadowFarDistSquared = 1.0f;
    size_t indexInFrame = 0;

    float shadowNearClipDist = 0.1f;
    float shadowFarClipDist = 300;

public:
    // Setters
    void setDiffuse(HUGO::Vector4 nDiff) { diffuse = nDiff; }
    void setSpecular(HUGO::Vector4 nSpec) { specular = nSpec; }
    void setLightType(HUGO::LightType lightT) { lightType = lightT; }
    void setSpotOuter(HUGO::Radian sOuter) { spotOuter = sOuter; }
    void setSpotInner(HUGO::Radian sInner) { spotInner = sInner; }
    void setSpotFallOff(float sFallOff) { spotFalloff = sFallOff; }
    void setSpotNearClip(float sNearClip) { spotNearClip = sNearClip; }
    void setAttenuation(HUGO::Vector4 nAtte) { attenuation = nAtte; }
    void setShadowFarDistance(float nShadowDistance) { shadowFarDist = nShadowDistance; }
    void setShadowFarDistanceSquared(float nShadowDistanceSquared) { shadowFarDistSquared = nShadowDistanceSquared; }
    void setIndexInFrame(size_t nIndexInFrame) { indexInFrame = nIndexInFrame; }
    void setShadowNearClipDist(float nShadowNearClipDist) { shadowNearClipDist = nShadowNearClipDist; }
    void setShadowFarClipDist(float nShadowFarClipDist) { shadowFarClipDist = nShadowFarClipDist; }

    // Getters
    HUGO::Vector4 getDiffuse() const { return diffuse; }
    HUGO::Vector4 getSpecular() const { return specular; }
    HUGO::LightType getLightType() const { return lightType; }
    HUGO::Radian getSpotOuter() const { return spotOuter; }
    HUGO::Radian getSpotInner() const { return spotInner; }
    float getSpotFallOff() const { return spotFalloff; }
    float getSpotNearClip() const { return spotNearClip; }
    HUGO::Vector4 getAttenuation() const { return attenuation; }
    float getShadowFarDistance() const { return shadowFarDist; }
    float getShadowFarDistanceSquared() const { return shadowFarDistSquared; }
    size_t getIndexInFrame() const { return indexInFrame; }
    float getShadowNearClipDistance() const { return shadowNearClipDist; }
    float getShadowFarClipDistance() const { return shadowFarClipDist; }

END_HCOMPONENT()
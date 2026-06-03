#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoComponentMacros.h"

HCOMPONENT(LightComponent)
private:
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, diffuse, HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, specular, HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::LightType, lightType, HUGO::LightType::LT_POINT);

    DEFAULT_SERIALIZED_MEMBER(float, spotFalloff, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(float, spotNearClip, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Radian, spotOuter, HUGO::Radian(0.7f));
    DEFAULT_SERIALIZED_MEMBER(HUGO::Radian, spotInner, HUGO::Radian(0.6f));

    // Ogre explanition, from the documentation reference:
    // range - The absolute upper range of the light in world units.
    // constant	- The constant factor in the attenuation formula: 1.0 means never attenuate, 0.0 is complete attenuation.
    // linear - The linear factor in the attenuation formula: 1 means attenuate evenly over the distance.
    // quadratic - The quadratic factor in the attenuation formula: adds a curvature to the attenuation formula.
    DEFAULT_SERIALIZED_MEMBER(float, shadowFarDist, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(float, shadowFarDistSquared, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, attenuation, HUGO::Vector4(1.0f, 0.5f, 0.0f, 10000.0f)); //range, constant, linear, quadratic

    size_t indexInFrame = 0;

    DEFAULT_SERIALIZED_MEMBER(float, shadowNearClipDist, 0.1f);
    DEFAULT_SERIALIZED_MEMBER(float, shadowFarClipDist, 300.0f);

public:
    // Setters
    inline void setDiffuse(const HUGO::Vector4& nDiff) { diffuse = nDiff; }
    inline void setSpecular(const HUGO::Vector4& nSpec) { specular = nSpec; }
    inline void setLightType(HUGO::LightType lightT) { lightType = lightT; }
    inline void setSpotOuter(HUGO::Radian sOuter) { spotOuter = sOuter; }
    inline void setSpotInner(HUGO::Radian sInner) { spotInner = sInner; }
    inline void setSpotFallOff(float sFallOff) { spotFalloff = sFallOff; }
    inline void setSpotNearClip(float sNearClip) { spotNearClip = sNearClip; }
    inline void setAttenuation(const HUGO::Vector4& nAtte) { attenuation = nAtte; }
    inline void setShadowFarDistance(float nShadowDistance) { shadowFarDist = nShadowDistance; }
    inline void setShadowFarDistanceSquared(float nShadowDistanceSquared) { shadowFarDistSquared = nShadowDistanceSquared; }
    inline void setIndexInFrame(size_t nIndexInFrame) { indexInFrame = nIndexInFrame; }
    inline void setShadowNearClipDist(float nShadowNearClipDist) { shadowNearClipDist = nShadowNearClipDist; }
    inline void setShadowFarClipDist(float nShadowFarClipDist) { shadowFarClipDist = nShadowFarClipDist; }

    // Getters
    inline HUGO::Vector4 getDiffuse() const { return diffuse; }
    inline HUGO::Vector4 getSpecular() const { return specular; }
    inline HUGO::LightType getLightType() const { return lightType; }
    inline HUGO::Radian getSpotOuter() const { return spotOuter; }
    inline HUGO::Radian getSpotInner() const { return spotInner; }
    inline float getSpotFallOff() const { return spotFalloff; }
    inline float getSpotNearClip() const { return spotNearClip; }
    inline HUGO::Vector4 getAttenuation() const { return attenuation; }
    inline float getShadowFarDistance() const { return shadowFarDist; }
    inline float getShadowFarDistanceSquared() const { return shadowFarDistSquared; }
    inline size_t getIndexInFrame() const { return indexInFrame; }
    inline float getShadowNearClipDistance() const { return shadowNearClipDist; }
    inline float getShadowFarClipDistance() const { return shadowFarClipDist; }

END_HCOMPONENT()


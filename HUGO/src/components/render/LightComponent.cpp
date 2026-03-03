#include "LightComponent.h"

LightComponent::LightComponent(EntityID entity
                , HUGO::Vector4 diff, HUGO::Vector4 specular, HUGO::LightType lType
                , const HUGO::Radian& outer, const HUGO::Radian& inner
                , float falloff, float nearClip, float shadowNearClipDist, float shadowFarClipDist)
    : Component(entity), diffuse(diff), specular(specular), lightType(lType), spotOuter(outer)
    , spotInner(inner), spotFalloff(falloff), spotNearClip(nearClip)
    , shadowNearClipDist(shadowNearClipDist), shadowFarClipDist(shadowFarClipDist) {}



#pragma region Setters
// ----------------------Setters----------------------
void LightComponent::setDiffuse(HUGO::Vector4 nDiff)
{
    diffuse = nDiff;
}
void LightComponent::setSpecular(HUGO::Vector4 nSpec)
{
    specular = nSpec;
}

void LightComponent::setLightType(HUGO::LightType lightT)
{
    lightType = lightT;
}

void LightComponent::setSpotOuter(HUGO::Radian sOuter)
{
    spotOuter = sOuter;
}

void LightComponent::setSpotInner(HUGO::Radian sInner)
{
    spotInner = sInner;
}

void LightComponent::setSpotFallOff(float sFallOff)
{
    spotFalloff = sFallOff;
}

void LightComponent::setSpotNearClip(float sNearClip)
{
    spotNearClip = sNearClip;
}

void LightComponent::setAttenuation(HUGO::Vector4 nAtte)
{
    attenuation = nAtte;
}

void LightComponent::setShadowFarDistance(float nShadowDistance)
{
    shadowFarDist = nShadowDistance;
}

void LightComponent::setShadowFarDistanceSquared(float nShadowDistanceSquared)
{
    shadowFarDistSquared = nShadowDistanceSquared;
}

void LightComponent::setIndexInFrame(size_t nIndexInFrame)
{
    indexInFrame = nIndexInFrame;
}

void LightComponent::setShadowNearClipDist(float nShadowNearClipDist)
{
    shadowNearClipDist = nShadowNearClipDist;   
}

void LightComponent::setShadowFarClipDist(float nShadowFarClipDist)
{
}


#pragma endregion

#pragma region Getters
// ----------------------Getters----------------------

HUGO::Vector4 LightComponent::getDiffuse() const
{
    return diffuse;
}

HUGO::Vector4 LightComponent::getSpecular() const
{
    return specular;
}

HUGO::LightType LightComponent::getLightType() const
{
    return lightType;
}

HUGO::Radian LightComponent::getSpotOuter() const
{
    return spotOuter;
}

HUGO::Radian LightComponent::getSpotInner() const
{
    return spotInner;
}

float LightComponent::getSpotFallOff() const
{
    return spotFalloff;
}

float LightComponent::getSpotNearClip() const
{
    return spotNearClip;
}

HUGO::Vector4 LightComponent::getAttenuation() const
{
    return attenuation;
}

float LightComponent::getShadowFarDistance() const
{
    return shadowFarDist;
}

float LightComponent::getShadowFarDistanceSquared() const
{
    return shadowFarDistSquared;
}

size_t LightComponent::getIndexInFrame() const
{
    return indexInFrame;
}

float LightComponent::getShadowNearClipDistance() const
{
    return shadowNearClipDist;
}

float LightComponent::getShadowFarClipDistance() const
{
    return shadowFarClipDist;
}


#pragma endregion

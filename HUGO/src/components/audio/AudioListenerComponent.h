#pragma once
#include "Component.h"
#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioListenerComponent)

    // velocity for Doppler calculations
    HUGO::Vector3 listenerVelocity;
    // master volume for this listener
    float masterVolume;

    AudioListenerComponent(EntityID entity,
                           const HUGO::Vector3& velocity = HUGO::Vector3(0,0,0),
                           float masterVolume = 1.0f)
        : Component(entity),
          listenerVelocity(velocity),
          masterVolume(masterVolume)
    {}

END_HCOMPONENT()
#pragma once
#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioListenerComponent)

    // velocity for Doppler calculations
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, listenerVelocity, HUGO::Vector3(0.0f))
    // master volume for this listener
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, masterVolume, 1.0f)
END_HCOMPONENT()


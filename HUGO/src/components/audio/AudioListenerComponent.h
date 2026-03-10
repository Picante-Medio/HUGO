#pragma once
#include "Component.h"
#include "HugoMath.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioListenerComponent)

    // velocity for Doppler calculations
    HUGO::Vector3 listenerVelocity = HUGO::Vector3(0,0,0);
    // master volume for this listener
    HUGO::Float masterVolume =1.0f;

END_HCOMPONENT()
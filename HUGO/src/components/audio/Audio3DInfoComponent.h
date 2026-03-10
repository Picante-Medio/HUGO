#pragma once
#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

// 3D SFX component
HCOMPONENT(Audio3DInfoComponent)

    HUGO::AudioID audioId = -1;

    // 3D attenuation distances
    HUGO::Float minDistance = 1.0f;
    HUGO::Float maxDistance = 100.0f;

    // movement in the world
    HUGO::Vector3 velocity=HUGO::Vector3(0.0f, 0.0f, 0.0f); // velocity for Doppler effect

    // source orientation
    HUGO::Vector3 direction=HUGO::Vector3(0.0f, 0.0f, 1.0f); // source direction (for cone)

    // doppler effect
    HUGO::Float dopplerFactor=1.0f;

    // rolloff
    HUGO::Float rolloff=1.0f; // how volume decreases with distance

    // emission cone
    HUGO::Float coneInnerAngle=360.f; // inner cone angle in degrees
    HUGO::Float coneOuterAngle=360.0f; // outer cone angle in degrees
    HUGO::Float coneOuterGain=0.0f; // gain outside the cone

END_HCOMPONENT()
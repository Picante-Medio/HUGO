#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

// 3D SFX component
HCOMPONENT(Audio3DInfoComponent)

    // 3D attenuation distances
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, minDistance, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, maxDistance, 100.0f)

    // movement in the world
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, velocity, HUGO::Vector3(0.0f)) // velocity for Doppler effect

    // source orientation
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, direction, HUGO::Vector3(0.0f, 0.0f, 1.0f)) // source direction (for cone)

    // doppler effect
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, dopplerFactor, 1.0f)

    // rolloff
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, rolloff, 1.0f) // how volume decreases with distance

    // emission cone
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, coneInnerAngle, 360.0f) // inner cone angle in degrees
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, coneOuterAngle, 360.0f) // outer cone angle in degrees
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, coneOuterGain, 0.0f) // gain outside the cone

END_HCOMPONENT()


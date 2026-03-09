#pragma once
#include "Component.h"
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

// 3D SFX component
HCOMPONENT(Audio3DInfoComponet)

    HUGO::AudioID audioId;

    // 3D attenuation distances
    float minDistance;
    float maxDistance;

    // movement in the world
    HUGO::Vector3 velocity; // velocity for Doppler effect

    // source orientation
    HUGO::Vector3 direction; // source direction (for cone)

    // doppler effect
    float dopplerFactor;

    // rolloff
    float rolloff; // how volume decreases with distance

    // emission cone
    float coneInnerAngle; // inner cone angle in degrees
    float coneOuterAngle; // outer cone angle in degrees
    float coneOuterGain; // gain outside the cone

    // Default constructor
    Audio3DInfoComponent(EntityID entity, HUGO::AudioID id, float minDistance= 1.0f, float maxDistance = 100.0f, 
        const HUGO::Vector3& velocity = HUGO::Vector3(0.0f, 0.0f, 0.0f), 
        const HUGO::Vector3& direction = HUGO::Vector3(0.0f, 0.0f, 1.0f),
        float dooplerFactor = 1.0f, float roolof=1.0f, 
        float coneInnerAngle = 360.0f, float coneOuterAngle = 360.0f, float coneOuterGain=0.0f): Component(entity), audioId(id),
          minDistance(minDistance), maxDistance(maxDistance), velocity(velocity), direction(direction), // default facing +Z
          dopplerFactor(dooplerFactor), rolloff(roolof),
          coneInnerAngle(coneInnerAngle), coneOuterAngle(coneOuterAngle), coneOuterGain(coneOuterGain){}

END_HCOMPONENT()
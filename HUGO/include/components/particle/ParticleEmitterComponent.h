#pragma once

#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

struct Particle {

    HUGO::String material = "";

    HUGO::Float width = 0.0f;
    HUGO::Float height = 0.0f;

};

HCOMPONENT(ParticleEmitterComponent)

    HUGO::String particleName = "";
    HUGO::Int quota = 10;
    bool sorted = true;

    HUGO::Vector3 direction = HUGO::Vector3(0.0f);
    
    HUGO::Float emissionRate = 6.0f;
    HUGO::Float timeToLive = 1.0f;
    HUGO::Float velocityMin = 0.0f;
    HUGO::Float velocityMax = 1.0f;
    HUGO::Vector4 colour = HUGO::Vector4(1.0f);
    HUGO::Float duration = 0.0f;   

    Particle pivot = {};
    bool active=false;
END_HCOMPONENT()

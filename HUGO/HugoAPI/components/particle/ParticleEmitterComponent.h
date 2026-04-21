#pragma once

#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(ParticleEmitterComponent)

    DEFAULT_SERIALIZED_MEMBER(HUGO::String, particleName, "");
    DEFAULT_SERIALIZED_MEMBER(HUGO::Int, quota, 10);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, sorted, true);

    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, direction, HUGO::Vector3(0.0f));
    
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, emissionRate, 6.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, timeToLive, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, velocityMin, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, velocityMax, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colour, HUGO::Vector4(1.0f))
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, duration, 0.0f);

    DEFAULT_SERIALIZED_MEMBER(HUGO::String, material, "");
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, width, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, height, 0.0f);

    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, emitting, false)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, active, false)

END_HCOMPONENT()


#pragma once

#include "HugoComponentMacros.h"
#include "HugoMath.h"
#include "HugoTypes.h"

HCOMPONENT(ParticleEmitterComponent)

    DEFAULT_SERIALIZED_MEMBER(std::string, particleName, "");
    DEFAULT_SERIALIZED_MEMBER(HUGO::Int, quota, 10);
    DEFAULT_SERIALIZED_MEMBER(bool, sorted, true);

    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector3, direction, HUGO::Vector3(0.0f));
    
    DEFAULT_SERIALIZED_MEMBER(float, emissionRate, 6.0f);
    DEFAULT_SERIALIZED_MEMBER(float, timeToLive, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(float, velocityMin, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, velocityMax, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Vector4, colour, HUGO::Vector4(1.0f))
    DEFAULT_SERIALIZED_MEMBER(float, duration, 0.0f);

    DEFAULT_SERIALIZED_MEMBER(std::string, material, "");
    DEFAULT_SERIALIZED_MEMBER(float, width, 0.0f);
    DEFAULT_SERIALIZED_MEMBER(float, height, 0.0f);

    DEFAULT_SERIALIZED_MEMBER(bool, emitting, false)
    DEFAULT_SERIALIZED_MEMBER(bool, active, false)

END_HCOMPONENT()


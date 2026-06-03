#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioComponent)
    DEFAULT_SERIALIZED_MEMBER(std::string, audioName, "");

    DEFAULT_SERIALIZED_MEMBER(bool, loop, false);
    DEFAULT_SERIALIZED_MEMBER(bool, streaming, false);
    DEFAULT_SERIALIZED_MEMBER(bool, isPlaying, false)

    DEFAULT_SERIALIZED_MEMBER(float, volume, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(float, pitch, 1.0f);
END_HCOMPONENT()
#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"

#include "HugoComponentMacros.h"

HCOMPONENT(AudioComponent)
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, audioName, "");

    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, loop, false);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, isPlaying, false)

    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, volume, 1.0f);
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, pitch, 1.0f);
END_HCOMPONENT()
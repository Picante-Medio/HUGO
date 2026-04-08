#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"
#include <functional>

#include "HugoComponentMacros.h"

struct ma_sound;
HCOMPONENT(AudioComponent)
    //serialization
    DEFAULT_SERIALIZED_MEMBER(HUGO::Int, type, HUGO::AudioTypes::LAST_TYPE)
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, audioName, "")
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, volume, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(bool, loop, false)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, pitch, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(double, startTime, 0.0)
    DEFAULT_SERIALIZED_MEMBER(bool, isPlaying, false)
    HUGO::AudioID audioId = -1;
    ma_sound* activePtr = nullptr; 
    std::function<void(HUGO::AudioID)> onEnd = nullptr;
    
END_HCOMPONENT()


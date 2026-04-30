#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"
#include <functional>

#include "HugoComponentMacros.h"

struct ma_sound;
HCOMPONENT(AudioComponent)
    //serialization
    DEFAULT_SERIALIZED_MEMBER(HUGO::AudioTypes, type, HUGO::AudioTypes::LAST_TYPE)
    DEFAULT_SERIALIZED_MEMBER(HUGO::String, audioName, "")
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, volume, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, loop, false)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Float, pitch, 1.0f)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Double, startTime, 0.0)
    DEFAULT_SERIALIZED_MEMBER(HUGO::Bool, isPlaying, false)
    
    HUGO::AudioID audioId = -1;
    ma_sound* activePtr = nullptr; 
    HUGO::Bool wasPlaying = false;
    std::function<void(HUGO::AudioID)> onEnd = nullptr;
    
END_HCOMPONENT()


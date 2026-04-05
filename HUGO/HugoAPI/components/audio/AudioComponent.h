#pragma once
#include "HugoMath.h"
#include "HugoTypes.h"
#include <functional>

#include "HugoComponentMacros.h"

struct ma_sound;
HCOMPONENT(AudioComponent)
    HUGO::AudioTypes type = HUGO::AudioTypes::LAST_TYPE;
    HUGO::AudioID audioId = -1;
    HUGO::String audioName = "";
    HUGO::Float volume= 1.0f;
    bool loop = false;
    HUGO::Float pitch = 1.0f; 
    // position of the audio in seconds to start to play
    double startTime=-1.0;

    ma_sound* activePtr = nullptr;
    // flag to know if this sound is playing or not
    bool isPlaying = false;

    // callback to be executed when the audio ends
    std::function<void(HUGO::AudioID)> onEnd = nullptr;

END_HCOMPONENT()


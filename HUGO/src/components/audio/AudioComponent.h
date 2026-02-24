#pragma once
#include "Component.h"
#include "HugoTypes.h"

struct AudioComponent: public Component
{
    HUGO::AudioTypes type;
    HUGO::AudioID soundId;
    float volume;
    bool loop;
    bool playOnStart;
    float pitch;
    // flag to know if this sound is playing or not
    bool isPlaying;
    AudioComponent(EntityID entity, HUGO::AudioTypes type, HUGO::AudioID soundId, float volume = 1.0f, bool loop = false, 
        bool playOnStart = false, float pitch = 1.0f)
        :Component(entity), soundId(soundId), volume(volume), loop(loop), 
        playOnStart(playOnStart), pitch(pitch), isPlaying(false){}
};
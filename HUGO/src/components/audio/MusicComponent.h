#pragma once
#include "Component.h"

// 2D SFX component
struct MusicComponent: public Component
{
    int soundId;
    float volume;
    bool loop;
    bool playOnStart;
    float pitch;
    // flag to know if this sound is playing or not
    bool isPlaying;
    MusicComponent(EntityID entity, int soundId, float volume = 1.0f, bool loop = false, 
        bool playOnStart = false, float pitch = 1.0f)
        :Component(entity), soundId(soundId), volume(volume), loop(loop), 
        playOnStart(playOnStart), pitch(pitch), isPlaying(false){}
};
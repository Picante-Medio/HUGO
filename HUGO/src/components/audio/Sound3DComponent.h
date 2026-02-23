#pragma once
#include "Component.h"
#include "HugoTypes.h"

// 3D SFX component
struct Sound3DComponent: public Component
{
    HUGO::AudioID soundId;
    float volume;
    bool loop;
    bool playOnStart;
    float minDistance;
    float maxDistance;
    // flag to know if this sound is playing or not
    bool isPlaying;

    Sound3DComponent(EntityID entity, int soundId, float volume = 1.0f, bool loop = false,
        bool playOnStart = false, float minDistance = 1.0f, float maxDistance = 10.0f):
        Component(entity), soundId(soundId), volume(volume), loop(loop), 
        playOnStart(playOnStart), minDistance(minDistance), maxDistance(maxDistance), isPlaying(false){}
};


#pragma once
#include "Component.h"
#include "HugoTypes.h"
#include <functional>

class ma_sound;
struct AudioComponent: public Component
{
    HUGO::AudioTypes type;
    HUGO::AudioID audioId;
    float volume;
    bool loop;
    bool playOnStart;
    float pitch;
    // position of the audio in seconds to start to play
    double startTime;

    ma_sound* activePtr;
    // flag to know if this sound is playing or not
    bool isPlaying;
    // bit flags to know if we changed some data
    HUGO::AudioDirtyFlags dirtyFlags;
    // callback to be executed when the audio ends
    std::function<void(HUGO::AudioID)> onEnd;

    AudioComponent(EntityID entity, HUGO::AudioTypes type, HUGO::AudioID audioId, float volume = 1.0f, bool loop = false, 
        bool playOnStart = false, float pitch = 1.0f, double startTime =0.0)
        :Component(entity), type(type),audioId(audioId), volume(volume), loop(loop), 
        playOnStart(playOnStart), pitch(pitch), startTime(startTime),
        activePtr(nullptr),isPlaying(false), dirtyFlags(HUGO::AudioDirtyFlags::NONE), onEnd(nullptr){}
};
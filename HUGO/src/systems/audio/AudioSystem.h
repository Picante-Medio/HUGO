#pragma once

#include "System.h"
#include "HugoTypes.h"
#include "miniaudio.h"
#include <vector>

class AudioSystem : public System
{
private:
    std::shared_ptr<ma_engine> engine;

    std::vector<ma_sound> sounds;
    std::vector<ma_sound> musics;

public:
    enum AudioTypes{SOUND2D, SOUND3D, MUSIC, LAST_TYPE};
    AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine);

    virtual ~AudioSystem();

    void update(double dt) override;
    // add an audio to the system
    // type determines if it's 2D, 3D or music
    void addAudio(const char* name, AudioTypes type);

    // playback control methods
    void playAudio(HUGO::AudioID id, AudioTypes type);
    void stopAudio(HUGO::AudioID id, AudioTypes type);
    void pauseAudio(HUGO::AudioID id, AudioTypes type);
    void resumeAudio(HUGO::AudioID id, AudioTypes type);

    // getters and setters functions
    void setAudioPosition(HUGO::AudioID id, AudioTypes type, double seconds); // set playback position in seconds
    double getAudioPosition(HUGO::AudioID id, AudioTypes type); // get current playback position in seconds
    void setLooping(HUGO::AudioID id, AudioTypes type, bool loop); // enable or disable looping for the audio
    void setVolume(HUGO::AudioID id, AudioTypes type, float vol); // set playback volume (0.0 = silent, 1.0 = full)
    
    // 3d audio functions
    void setPosition(HUGO::AudioID id, float x, float y, float z);
    void setVelocity(HUGO::AudioID id, float vx, float vy, float vz);
    void setDirection(HUGO::AudioID id, float dx, float dy, float dz);

    
};

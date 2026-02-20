#pragma once
#include "System.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "HugoTypes.h"
#include <unordered_map>

class AudioSystem : public System{
private:
    std::shared_ptr<ma_engine> engine;
    std::vector<ma_sound> sounds;
    std::vector<ma_sound> musics; 

    //functions
    void playAudio(HUGO::MusicID id);
public:
    AudioSystem(std::shared_ptr<Scene> scene);
    virtual ~AudioSystem();
    void update(double dt) override;
};
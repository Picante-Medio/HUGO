#pragma once
#include "System.h"
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "../utils/HugoTypes.h"
#include <unordered_map>

class AudioSystem : public System{
private:
    std::shared_ptr<ma_engine> engine;
    std::unordered_map<HUGO::MusicID, ma_sound> sound;

    //functions
    void playAudio(HUGO::MusicID id);
public:
    AudioSystem(std::shared_ptr<Scene> scene);
    virtual ~AudioSystem();
    void update(double dt) override;
};
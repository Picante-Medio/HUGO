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
    AudioSystem(std::shared_ptr<Scene> scene);

    virtual ~AudioSystem();

    void update(double dt) override;

    void playAudio(HUGO::MusicID id, bool sfx);
    void stopAudio(HUGO::MusicID id, bool sfx);
};

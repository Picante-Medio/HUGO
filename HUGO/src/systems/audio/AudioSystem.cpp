#include "AudioSystem.h"

#include <stdexcept>
#include <iostream>
#include <cassert>

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine)
    : System(scene), engine(maEngine),sounds(), musics()
{
   
    sounds.emplace_back();

    ma_result result = ma_sound_init_from_file(
        this->engine.get(),
        "lucky.mp3",
        MA_SOUND_FLAG_NO_SPATIALIZATION,
        nullptr,
        nullptr,
        &sounds.back()
    );

    if (result != MA_SUCCESS)
        throw std::runtime_error("Failed to load lucky.mp3");

    std::cout << "Audio loaded correctly\n";
}

AudioSystem::~AudioSystem()
{
    for (auto& s : sounds)  ma_sound_uninit(&s);

    for (auto& m : musics) ma_sound_uninit(&m); 
}

void AudioSystem::update(double dt)
{
    // Aquí podrías gestionar timers si quieres parar sonidos tras X segundos
}

void AudioSystem::playAudio(HUGO::AudioID id, AudioTypes type)
{
    //its a sound effect
    if (type < MUSIC){
       if (id <0 || id >= sounds.size())
            return;

        ma_sound_start(&sounds[id]);
    }
    else{
        if (id <0 || id >= musics.size())
            return;

        ma_sound_start(&musics[id]);
    }
}

void AudioSystem::stopAudio(HUGO::AudioID id, AudioTypes type)
{
    if (id >= sounds.size())
        return;

    ma_sound_stop(&sounds[id]);
}

void AudioSystem::addAudio(const char* name, AudioTypes type)
{
    ma_result result;
    switch (type)
    {
        case SOUND2D:
            sounds.emplace_back();
            result = ma_sound_init_from_file(
                this->engine.get(),
                name,
                MA_SOUND_FLAG_NO_SPATIALIZATION,
                nullptr,
                nullptr,
                &sounds.back()
            );
            break;
        case SOUND3D:
            sounds.emplace_back();
            result = ma_sound_init_from_file(
                this->engine.get(),
                name,
                MA_SOUND_FLAG_DECODE, //a default miniaudio sound is a 3d sound
                nullptr,
                nullptr,
                &sounds.back()
            );
            break;
        case MUSIC:
            musics.emplace_back();
            result = ma_sound_init_from_file(
                this->engine.get(),
                name,
                MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM,
                nullptr,
                nullptr,
                &musics.back()
            );
            break;
        default:
            break;
    }
}

void AudioSystem::pauseAudio(HUGO::AudioID id, AudioTypes type)
{
}

void AudioSystem::resumeAudio(HUGO::AudioID id, AudioTypes type)
{
}

void AudioSystem::setAudioPosition(HUGO::AudioID id, AudioTypes type, double seconds)
{
}

double AudioSystem::getAudioPosition(HUGO::AudioID id, AudioTypes type)
{
    return 0.0;
}

void AudioSystem::setLooping(HUGO::AudioID id, AudioTypes type, bool loop)
{
}

void AudioSystem::setVolume(HUGO::AudioID id, AudioTypes type, float vol)
{
}

void AudioSystem::setPosition(HUGO::AudioID id, float x, float y, float z)
{
}

void AudioSystem::setVelocity(HUGO::AudioID id, float vx, float vy, float vz)
{
}

void AudioSystem::setDirection(HUGO::AudioID id, float dx, float dy, float dz)
{
}

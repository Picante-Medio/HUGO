#include "AudioSystem.h"

#include <stdexcept>
#include <iostream>
#include <cassert>

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene)
    : System(scene), sounds(), musics()
{
    engine = std::make_shared<ma_engine>();
    ma_result result = ma_engine_init(nullptr, engine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";

    sounds.emplace_back();

    result = ma_sound_init_from_file(
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
    // Liberar sonidos correctamente (POR REFERENCIA)
    for (auto& s : sounds)
    {
        ma_sound_uninit(&s);
    }

    for (auto& m : musics)
    {
        ma_sound_uninit(&m);
    }

    // ❌ NO desinicializar el engine aquí
    // El engine se destruye donde se creó (main)
}

void AudioSystem::update(double dt)
{
    // Aquí podrías gestionar timers si quieres parar sonidos tras X segundos
}

void AudioSystem::playAudio(HUGO::MusicID id, bool sfx)
{
    if (id >= sounds.size())
        return;

    ma_sound_start(&sounds[id]);
}

void AudioSystem::stopAudio(HUGO::MusicID id, bool sfx)
{
    if (id >= sounds.size())
        return;

    ma_sound_stop(&sounds[id]);
}
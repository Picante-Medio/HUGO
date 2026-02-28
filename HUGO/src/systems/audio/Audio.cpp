#include "Audio.h"

Audio::Audio(Audio &&other) noexcept
{
    audio = other.audio;
    initialized = other.initialized;
    other.initialized = false;
}

Audio &Audio::operator=(Audio &&other) noexcept
{
    if (this != &other)
        {
            if (initialized)
                ma_sound_uninit(&audio);

            audio = other.audio;
            initialized = other.initialized;
            other.initialized = false;
        }
        return *this;
}

Audio::~Audio()
{
    if (initialized)
    ma_sound_uninit(&audio);
}

ma_sound *Audio::get()
{
    return &audio; 
}

bool Audio::init(ma_engine *engine, const char *filename, ma_uint32 flags)
{
    ma_result result = ma_sound_init_from_file(
        engine,
        filename,
        flags,
        nullptr,
        nullptr,
        &audio
    );

    initialized = (result == MA_SUCCESS);
    return initialized;
}

void Audio::rewind()
{
    ma_sound_seek_to_pcm_frame(&audio, 0);
}

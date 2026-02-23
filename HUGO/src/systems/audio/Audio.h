// wrapper for audios
#pragma once
#include "miniaudio.h"


class Audio {
private:
    ma_sound audio;

public:
    Audio() = default;

    ~Audio() {
        ma_sound_uninit(&audio);
    }

    ma_sound* get() { return &audio; }

    // Initialize from file with given flags
    inline bool init(ma_engine* engine, const char* filename, ma_uint32 flags) {
        ma_result result = ma_sound_init_from_file(
            engine,
            filename,
            flags,
            nullptr,
            nullptr,
            &audio
        );
        return result == MA_SUCCESS;
    }
};

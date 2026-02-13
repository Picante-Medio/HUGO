#pragma once
#include "miniaudio.h"

class Audio {
private:
    ma_sound audio{};
    bool initialized = false;

public:
    Audio() = default;

    // it CANT be copied
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    // but it can be moved
    Audio(Audio&& other) noexcept;
    Audio& operator=(Audio&& other) noexcept;

    virtual ~Audio();

    ma_sound* get();

    bool init(ma_engine* engine, const char* filename, ma_uint32 flags = 0);

    void rewind();
};
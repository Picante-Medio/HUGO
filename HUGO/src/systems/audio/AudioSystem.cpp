#include "AudioSystem.h"
#include <iostream>
#include <stdexcept>
AudioSystem::AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine)
    : System(scene), engine(maEngine), audios(), nextID(0)
{
    std::cout << "AudioSystem created\n";
    addAudio("lucky.mp3", MUSIC);
    playAudio(0);
    setVolume(0, 0.1f);

}

void AudioSystem::update(double dt)
{
}

void AudioSystem::fixedUpdate(double dt)
{
}

AudioInfo AudioSystem::createAudio(const char* filename, AudioTypes type)
{
    AudioInfo audio;
    audio.ptr = std::make_unique<Audio>();
    ma_uint32 flags = 0;

    //choose the flags depending on what type of audio it is
    switch (type) {
        case SOUND2D: 
            flags = MA_SOUND_FLAG_NO_SPATIALIZATION; 
            break;
        case SOUND3D: 
            flags = 0; 
            break;
        case MUSIC:
            flags = MA_SOUND_FLAG_NO_SPATIALIZATION | MA_SOUND_FLAG_STREAM; 
            break;
        default: 
            throw std::runtime_error("Error createAudio: unknown AudioType");
            break;
    }
    if (!audio.ptr->init(engine.get(), filename, flags))
        throw std::runtime_error(std::string("Failed to load ") + filename);
    audio.type = type;
    return audio;
}

ma_sound* AudioSystem::getSound(HUGO::AudioID id)
{
   if (id < 0 || id >= audios.size()) 
    return nullptr;
   return audios[id].ptr->get();
}

ma_sound *AudioSystem::get3DSound(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    if (!s || audios[id].type != SOUND3D) return nullptr;
    else return s;
}


void AudioSystem::addAudio(const char* name, AudioTypes type)
{
    audios.emplace_back(createAudio(name, type));
    ++nextID;
}

void AudioSystem::playAudio(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    if (!s) return;

    ma_sound_start(s);
}

void AudioSystem::stopAudio(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    if (!s) return;

    ma_sound_stop(s);
}

void AudioSystem::resumeAudio(HUGO::AudioID id)
{
    playAudio(id);
}

void AudioSystem::setAudioPosition(HUGO::AudioID id, double seconds)
{
    ma_sound* s = getSound(id);
    if (!s) return;

    if (seconds < 0.0) seconds = 0.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    ma_uint64 targetFrame = static_cast<ma_uint64>(seconds * sr);

    ma_uint64 totalFrames = 0;
    if (ma_sound_get_length_in_pcm_frames(s, &totalFrames) == MA_SUCCESS && targetFrame > totalFrames)
        targetFrame = totalFrames;

    ma_sound_seek_to_pcm_frame(s, targetFrame);
}

double AudioSystem::getAudioPosition(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    if (!s) return -1.0;

    ma_uint64 cursor = 0;
    if (ma_sound_get_cursor_in_pcm_frames(s, &cursor) != MA_SUCCESS)
        return -1.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    return static_cast<double>(cursor) / sr;
}

void AudioSystem::setLooping(HUGO::AudioID id, bool loop)
{
    ma_sound* s = getSound(id);
    if (!s) return;

    ma_sound_set_looping(s, loop);
}

void AudioSystem::setVolume(HUGO::AudioID id, float vol)
{
    ma_sound* sound = getSound(id);
    if (!sound) return;

    //multiply factor according to its type
    if(audios[id].type < MUSIC)
        vol *= soundsGroupVolume;
    else
        vol *= musicsGroupVolume;

    ma_sound_set_volume(sound, vol);
}

void AudioSystem::setGroupVolume(AudioTypes type, float volume)
{
    if (type < MUSIC)
        soundsGroupVolume = volume; // O(1)
    else
        musicsGroupVolume = volume;  // O(1)
}

void AudioSystem::setPosition(HUGO::AudioID id, float x, float y, float z)
{
    ma_sound* s = get3DSound(id);
    ma_sound_set_position(s, x, y, z);
}

void AudioSystem::setVelocity(HUGO::AudioID id, float vx, float vy, float vz)
{
    ma_sound* s = get3DSound(id);
    ma_sound_set_velocity(s, vx, vy, vz);
}

void AudioSystem::setDirection(HUGO::AudioID id, float dx, float dy, float dz)
{
    ma_sound* s = get3DSound(id);
    ma_sound_set_direction(s, dx, dy, dz);
}

void AudioSystem::setDopplerFactor(HUGO::AudioID id, float factor)
{
    ma_sound* s = get3DSound(id);
    ma_sound_set_doppler_factor(s, factor);
}

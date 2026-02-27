#include "AudioSystem.h"

#include "AudioComponent.h"
#include "Audio3DInfoComponet.h"
#include "Scene.h"
#include "TransformComponent.h"

#include <fstream>
#include <iostream>
#include <list>
#include <stdexcept>
#include <sstream>

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine, const std::string& path)
    : System(scene), engine(maEngine), path(path), audios(), nextID(0)
{
    std::string completePath = path + "AudioList.txt";
    loadFromFile(completePath.c_str());
}

// HELPERS
void AudioSystem::loadFromFile(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open audio file list");

    std::string line;
    while (std::getline(file, line))
    {
        // skip empty lines or lines that start with #
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string id, filename, typeStr;

        std::getline(ss, id, ';');
        std::getline(ss, filename, ';');
        std::getline(ss, typeStr, ';');

        HUGO::AudioTypes type = static_cast<HUGO::AudioTypes>(std::stoi(typeStr));

        audios.emplace_back(createAudio(filename.c_str(), type));
        audioMap[id] = nextID;
        nextID++;
    }
    file.close();
}

AudioInfo AudioSystem::createAudio(const char* filename, HUGO::AudioTypes type)
{
    AudioInfo audio;
    audio.ptr = std::make_unique<Audio>();
    ma_uint32 flags = 0;

    //choose the flags depending on what type of audio it is
    switch (type) {
        case HUGO::SOUND2D: 
            flags = MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION; 
            break;
        case HUGO::SOUND3D: 
            flags = MA_SOUND_FLAG_DECODE; 
            break;
        case HUGO::MUSIC:
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
    if (id < 0 || id >= static_cast<HUGO::AudioID>(audios.size()))
        throw std::range_error("Error getSound: invalid index");

    ma_sound* sound = audios[id].ptr ? audios[id].ptr->get() : nullptr;

    if (sound == nullptr) 
        throw std::runtime_error("Error getSound: Could not find the audio");

    return sound;
}

ma_sound *AudioSystem::get3DSound(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    if (audios[id].type != HUGO::SOUND3D)
        throw std::runtime_error("Error get3DSound: This is not a 3D sound");
    else return s;
}

void AudioSystem::processChanges(AudioComponent &c)
{
    if (c.dirtyFlags == HUGO::AudioDirtyFlags::NONE) return;

    HUGO::AudioID id = c.audioId;
    ma_sound* s = getSound(id);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::LOOP)
        setLooping(s, c.loop);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::VOLUME)
        setVolume(id, c.volume);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::PITCH)
        setPitch(s, c.pitch);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::STARTTIME)
        setAudioPosition(s, c.startTime);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::START)
        playAudio(s);

    if (c.dirtyFlags & HUGO::AudioDirtyFlags::STOP)
        stopAudio(s);

    c.dirtyFlags = HUGO::AudioDirtyFlags::NONE; // clean the flags after finishing
}

void AudioSystem::process3DChanges(Audio3DInfoComponet &c, const TransformComponent& t)
{
    if (c.dirtyFlags == HUGO::Audio3DDirtyFlags::NONE_3D) return;

    HUGO::AudioID id = c.audioId;
    ma_sound* s = getSound(id);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::MINDISTANCE)
        set3DMinDistance(s, c.minDistance);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::MAXDISTANCE)
        set3DMaxDistance(s, c.maxDistance);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::VELOCITY)
        setVelocity(s, c.velocity);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::DIRECTION)
        setDirection(s, c.direction);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::DOPPLER)
        setDopplerFactor(s, c.dopplerFactor);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::ROLLOF)
        set3DRollof(s, c.rolloff);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::CONE)
        set3DCone(s, c.coneInnerAngle, c.coneOuterAngle, c.coneOuterGain);

    if (c.dirtyFlags & HUGO::Audio3DDirtyFlags::MOVED){
        set3DPosition(s, t.position);
    }
    c.dirtyFlags = HUGO::Audio3DDirtyFlags::NONE_3D; // clean the flags after finishing
}

void AudioSystem::playAudio(ma_sound* s)
{
    ma_sound_start(s);
}

void AudioSystem::playAudio(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    playAudio(s);
    // we need to connect the callback
    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& c = pack.get<AudioComponent&>();
        if (c.audioId == id && c.onEnd != nullptr) {
            ma_sound_set_end_callback(s, &AudioSystem::endCallback, &c);
            break;
        }
    }
}

void AudioSystem::stopAudio(ma_sound* s)
{
    ma_sound_stop(s);
}

void AudioSystem::resumeAudio(ma_sound* s)
{
    playAudio(s);
}

void AudioSystem::setAudioPosition(ma_sound* s, double seconds)
{
    if (seconds < 0.0) seconds = 0.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    ma_uint64 targetFrame = static_cast<ma_uint64>(seconds * sr);

    ma_uint64 totalFrames = 0;
    if (ma_sound_get_length_in_pcm_frames(s, &totalFrames) == MA_SUCCESS && targetFrame > totalFrames)
        targetFrame = totalFrames;

    ma_sound_seek_to_pcm_frame(s, targetFrame);
}

double AudioSystem::getAudioPosition(ma_sound* s)
{
    ma_uint64 cursor = 0;
    if (ma_sound_get_cursor_in_pcm_frames(s, &cursor) != MA_SUCCESS)
        return -1.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    return static_cast<double>(cursor) / sr;
}

void AudioSystem::setLooping(ma_sound* s, bool loop)
{
    ma_sound_set_looping(s, loop);
}

void AudioSystem::setVolume(HUGO::AudioID id, float vol)
{
    ma_sound* sound = getSound(id);
    if (!sound) return;

    //multiply factor according to its type
    if(audios[id].type < HUGO::MUSIC)
        vol *= soundsGroupVolume;
    else
        vol *= musicsGroupVolume;

    ma_sound_set_volume(sound, vol);
}

void AudioSystem::setPitch(ma_sound* s, float pitch)
{
    ma_sound_set_pitch(s, pitch);
}

void AudioSystem::setGroupVolume(HUGO::AudioTypes type, float volume)
{
    if (type < HUGO::MUSIC)
        soundsGroupVolume = volume; // O(1)
    else
        musicsGroupVolume = volume;  // O(1)
}

void AudioSystem::set3DPosition(ma_sound* s, const HUGO::Vector3& p)
{
    ma_sound_set_position(s, p.x, p.y, p.z);
}

void AudioSystem::setVelocity(ma_sound* s, const HUGO::Vector3& v)
{
    ma_sound_set_velocity(s, v.x, v.y, v.z);
}

void AudioSystem::setDirection(ma_sound* s, const HUGO::Vector3& d)
{
    ma_sound_set_direction(s, d.x, d.y, d.z);
}

void AudioSystem::setDopplerFactor(ma_sound* s, float factor)
{
    ma_sound_set_doppler_factor(s, factor);
}

void AudioSystem::set3DMinDistance(ma_sound *s, float distance)
{
    ma_sound_set_min_distance(s, distance);
}

void AudioSystem::set3DMaxDistance(ma_sound *s, float distance)
{
    ma_sound_set_max_distance(s, distance);
}

void AudioSystem::set3DRollof(ma_sound *s, float rollof)
{
    ma_sound_set_rolloff(s, rollof);
}

void AudioSystem::set3DCone(ma_sound *s, float innerAngle, float outerAngle, float outerGain)
{
    ma_sound_set_cone(s, innerAngle, outerAngle, outerGain);
}

void AudioSystem::endCallback(void* data, ma_sound* pSound)
{
    AudioComponent* c = static_cast<AudioComponent*>(data);
    if (!c) return;

    c->isPlaying = false;
    c->dirtyFlags = HUGO::STOP;

    if (c->onEnd){
        c->onEnd(c->audioId);  // call lambda expression
    }
}

// PUBLIC METHODS
void AudioSystem::update(double dt)
{
    elapsedTime += dt;

    for (auto& pack : scene->query<Audio3DInfoComponet, TransformComponent>()) {
        Audio3DInfoComponet& sc = pack.get<Audio3DInfoComponet&>();
        TransformComponent& tr = pack.get<TransformComponent&>();
        process3DChanges(sc, tr);
    }


    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& sc = pack.get<AudioComponent&>();

        // state 1
        if (!stopTriggered && elapsedTime >= x)
        {
            sc.isPlaying = false;
            sc.dirtyFlags = HUGO::STOP;

            stopTriggered = true;
        }

        // state 2
        if (!startTriggered && elapsedTime >= 2.0 * x)
        {
            sc.isPlaying = true;
            sc.pitch = 0.75f;
            sc.startTime = 10.0;

            sc.dirtyFlags = static_cast<HUGO::AudioDirtyFlags>(HUGO::PITCH | HUGO::STARTTIME | HUGO::START);

            startTriggered = true;
        }

        // apply changes
        if (sc.dirtyFlags != HUGO::NONE)
            processChanges(sc);
    }
}

void AudioSystem::fixedUpdate(double dt)
{
}

void AudioSystem::addAudio(const char* name, HUGO::AudioTypes type)
{
    audios.emplace_back(createAudio(name, type));
    ++nextID;
}

void AudioSystem::createTestEntity()
{
    EntityID ent = scene->createEntity();
    scene->addComponent<AudioComponent>(ent, HUGO::MUSIC, audioMap["bgm"]);
    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& sc = pack.get<AudioComponent&>();
        sc.isPlaying = true;
        sc.loop = true;
        sc.pitch = 2.0f;
        sc.startTime = 30.0;
        sc.volume = 0.1f;
        sc.dirtyFlags = static_cast<HUGO::AudioDirtyFlags>(
                        HUGO::AudioDirtyFlags::LOOP |
                        HUGO::AudioDirtyFlags::PITCH |
                        HUGO::AudioDirtyFlags::STARTTIME |
                        HUGO::AudioDirtyFlags::START |
                        HUGO::AudioDirtyFlags::VOLUME);
    }
}
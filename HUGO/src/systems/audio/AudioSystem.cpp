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

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine, 
    const std::string& path, int queueSize)
    : System(scene), engine(maEngine), path(path), audios(), nameToID(), pools(), nextID(0), queueSize(queueSize)
{
    std::string completePath = path + "AudioList.txt";
    loadFromFile(completePath.c_str());
    
}

// HELPERS
void AudioSystem::loadFromFile(const char* path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open audio file list");

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);

        std::string name, filename, typeStr;

        std::getline(ss, name, ';'); // name of the audio
        std::getline(ss, filename, ';'); // file root
        std::getline(ss, typeStr, ';'); // type of audio

        HUGO::AudioTypes type =
            static_cast<HUGO::AudioTypes>(std::stoi(typeStr));

        HUGO::AudioID id = nextID++;

        audios.push_back({name, filename, type});
        nameToID[name] = id;

        pools.emplace_back(); // pool aligned with id
    }
}

AudioInfo AudioSystem::createVoice(HUGO::AudioID id)
{
    AudioInfo info;
    info.ptr = std::make_unique<Audio>();

    std::string fullPath = audios[id].filename;

    if (!info.ptr->init(engine.get(), fullPath.c_str(), 0))
        throw std::runtime_error("Failed loading audio");

    return info;
}

ma_sound* AudioSystem::getSound(HUGO::AudioID id)
{
    AudioPool& pool = pools[id];

    // reuse free voice
    // O(N) but N is a small number
    for (auto& voice : pool.voices)
    {
        if (!voice.inUse)
        {
            voice.inUse = true;
            return voice.ptr->get();
        }
    }

    // create new if it is allowed
    if (pool.voices.size() < queueSize)
    {
        pool.voices.emplace_back(createVoice(id));

        AudioInfo& v = pool.voices.back();
        v.inUse = true;

        ma_sound_set_end_callback(
            v.ptr->get(),
            AudioSystem::endCallback,
            nullptr
        );

        return v.ptr->get();
    }

    // else, we need to steal a position (round robin)
    AudioInfo& stolen = pool.voices[pool.stealIndex];
    pool.stealIndex =
        (pool.stealIndex + 1) % pool.voices.size();

    ma_sound_stop(stolen.ptr->get());
    ma_sound_seek_to_pcm_frame(stolen.ptr->get(), 0);

    stolen.inUse = true;

    return stolen.ptr->get();
}

ma_sound *AudioSystem::getExistingSound(AudioComponent &c)
{
    return c.activePtr; 
}

ma_sound *AudioSystem::acquireVoice(AudioComponent &c)
{
    if (!c.activePtr)
    {
        c.activePtr = getSound(c.audioId);

        if (c.onEnd)
        {
            ma_sound_set_end_callback(
                c.activePtr,
                &AudioSystem::endCallback,
                &c
            );
        }
    }

    return c.activePtr;
}

void AudioSystem::processChanges(AudioComponent& c)
{
    // if nothing changed in this component, return
    if (c.dirtyFlags == HUGO::NONE)
        return;

    // get the pointer to the currently active voice (if it has)
    ma_sound* s = c.activePtr;

    // acquire a voice ONLY when playback is requested
    // this avoids creating EXTRA sounds for simple parameter updates
    if (!s && (c.dirtyFlags & HUGO::START))
    {
        s = getSound(c.audioId);
        c.activePtr = s;
    }

    // if we still don't have a valid voice,
    // there is nothing we can apply changes to
    if (!s)
    {
        c.dirtyFlags = HUGO::NONE;
        return;
    }

    if (c.dirtyFlags & HUGO::LOOP)
        setLooping(s, c.loop);

    
    if (c.dirtyFlags & HUGO::VOLUME)
        setVolume(c.audioId, c.volume);

    if (c.dirtyFlags & HUGO::PITCH)
        setPitch(s, c.pitch);

    if (c.dirtyFlags & HUGO::STARTTIME)
        setAudioPosition(s, c.startTime);

    if (c.dirtyFlags & HUGO::START)
        playAudio(s);

    if (c.dirtyFlags & HUGO::STOP)
    {
        // stop playback and release the active voice reference
        stopAudio(s);
    }

    // clear flags
    c.dirtyFlags = HUGO::NONE;
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
    if (id < 0 || id >= pools.size()) return;

    AudioPool& pool = pools[id];

    // apply group volume
    if (audios[id].type < HUGO::MUSIC)
        vol *= soundsGroupVolume;
    else
        vol *= musicsGroupVolume;

    // apply volume to all active voices
    // O(N)
    for (auto& voice : pool.voices)
    {
        if (voice.inUse)
            ma_sound_set_volume(voice.ptr->get(), vol);
    }
}

void AudioSystem::setPitch(ma_sound* s, float pitch)
{
    ma_sound_set_pitch(s, pitch);
}

void AudioSystem::setGroupVolume(HUGO::AudioTypes type, float vol)
{
    if (type < HUGO::MUSIC)
        soundsGroupVolume = vol; // O(1)
    else
        musicsGroupVolume = vol;  // O(1)
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

    int i =0;
    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& sc = pack.get<AudioComponent&>();
        if (i>0){
            processChanges(sc);
            continue;
        }
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
        
        ++i;
    }

    //the correct version of the update loop 
    // for (auto& pack : scene->query<Audio3DInfoComponet, TransformComponent>()) {
    //     Audio3DInfoComponet& sc = pack.get<Audio3DInfoComponet&>();
    //     TransformComponent& tr = pack.get<TransformComponent&>();
    //     process3DChanges(sc, tr);
    // }

    // for (auto& pack : scene->query<AudioComponent>()) {
    //     AudioComponent& sc = pack.get<AudioComponent&>();
    //     processChanges(sc);
    // }
}

void AudioSystem::fixedUpdate(double dt)
{
}

void AudioSystem::addAudio(const char* name, HUGO::AudioTypes type)
{
    std::string audioName = name;

    auto it = nameToID.find(audioName);
    if (it == nameToID.end())
        throw std::runtime_error("Audio already registered");

    HUGO::AudioID id = nextID++;

    audios.push_back({audioName, audioName, type});
    nameToID[audioName] = id;

    pools.emplace_back();
}

void AudioSystem::createTestEntity()
{
    EntityID ent = scene->createEntity();
    EntityID ent2 = scene->createEntity();
    scene->addComponent<AudioComponent>(ent, HUGO::MUSIC, nameToID["bgm"]);
    scene->addComponent<AudioComponent>(ent2, HUGO::MUSIC, nameToID["bgm"]);
    int i = 0;
    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& sc = pack.get<AudioComponent&>();
        if (i == 0){
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
        else if (i == 1){
            sc.isPlaying = true;
            sc.volume = 0.05f;
            sc.dirtyFlags = static_cast<HUGO::AudioDirtyFlags>(
                            HUGO::AudioDirtyFlags::START |
                            HUGO::AudioDirtyFlags::VOLUME);
        }
        ++i;
    }
}
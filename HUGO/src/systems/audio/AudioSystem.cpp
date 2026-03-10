#include "AudioSystem.h"

#include "AudioComponent.h"
#include "AudioListenerComponent.h"
#include "Audio3DInfoComponent.h"

#include "Scene.h"
#include "TransformComponent.h"
#include "LuaBinder.h"

#include <fstream>
#include <sstream>

AudioSystem::AudioSystem(std::shared_ptr<Scene> scene, std::shared_ptr<ma_engine> maEngine, 
    const std::string& path, int queueSize)
    : System(scene), engine(maEngine), path(path), audios(), nameToID(), pools(), nextID(0), queueSize(queueSize)
{   
    LuaBinder::Instance().registerMethod<AudioSystem, void, int>(*this, &AudioSystem::playAudioLUA, "playAudio");
}

ErrorOrType<AudioInfo> AudioSystem::createVoice(HUGO::AudioID id)
{
    AudioInfo info;
    info.ptr = std::make_shared<Audio>();

    std::string fullPath = path + "/" + audios[id].filename;

    if (!info.ptr->init(engine.get(), fullPath.c_str(), 0))
    {
        StringError err;
        err.errorMessage = "Failed loading audio";
        return ErrorOrType<AudioInfo>(err);
    }

    // configuration according to their type
    if (audios[id].type == HUGO::SOUND3D)
    {
        ma_sound_set_spatialization_enabled(info.ptr->get(), MA_TRUE);
        ma_sound_set_attenuation_model(info.ptr->get(), ma_attenuation_model_inverse);
    }
    else
    {
        ma_sound_set_spatialization_enabled(info.ptr->get(), MA_FALSE);
    }

    return ErrorOrType<AudioInfo>(std::move(info));
}

// voice pool functions

/**
 * @brief get an available sound instance from the pool.
 *
 * The system first tries to reuse a free voice from the pool. If none are
 * available, it will create a new voice as long as the pool has not reached
 * the maximum queue size. If the pool is full, a voice will be reused using
 * a round-robin stealing strategy.
 *
 * @param id Audio identifier used to select the corresponding audio pool.
 * @return Pointer to the acquired ma_sound instance.
 */
ma_sound* AudioSystem::getSound(HUGO::AudioID id)
{
    AudioPool& pool = pools[id];

    // STEP 1 : try reusing an existing free voice
    // O(N), but N is small (queueSize)
    for (auto& voice : pool.voices)
    {
        if (!voice.inUse)
        {
            voice.inUse = true;
            // reset playback to the beginning
            ma_sound_seek_to_pcm_frame(voice.ptr->get(), 0);
            return voice.ptr->get();
        }
    }

     // STEP 2 : create a new voice if we haven't reached the pool limit yet
    if (pool.voices.size() < static_cast<size_t>(queueSize))
    {   
        // try to create a new voice
        ErrorOrType<AudioInfo> result = createVoice(id);

        if (result.isError())
            return nullptr;

        // add the new voice to the pool (movement)
        pool.voices.emplace_back(std::move(result.getValue()));

        // reference to the newly created voice
        AudioInfo& v = pool.voices.back();
        v.inUse = true;

        ma_sound_set_end_callback(
            v.ptr->get(),
            AudioSystem::endCallback,
            &v);

        return v.ptr->get();
    }

    AudioInfo& stolen = pool.voices[pool.stealIndex];
    pool.stealIndex = (pool.stealIndex + 1) % pool.voices.size();

    ma_sound_stop(stolen.ptr->get());
    ma_sound_seek_to_pcm_frame(stolen.ptr->get(), 0);

    stolen.inUse = true;

    return stolen.ptr->get();
}

// ensures a component has a valid voice
// only creates one if it doesn't exist yet
ma_sound *AudioSystem::acquireVoice(AudioComponent &c)
{
    if (c.activePtr == nullptr) 
        c.activePtr = getSound(c.audioId);
    return c.activePtr;
}

// CALLBACK
void AudioSystem::endCallback(void* data, ma_sound* sound)
{
    if (!data) return;

    AudioInfo& voice = *reinterpret_cast<AudioInfo*>(data);

    voice.inUse = false;
}

#pragma region PLAYBACK CONTROLS
void AudioSystem::playAudio(ma_sound* s)
{
    if (s)
        ma_sound_start(s);
}

void AudioSystem::playAudio(HUGO::AudioID id)
{
    ma_sound* s = getSound(id);
    playAudio(s);
}

void AudioSystem::stopAudio(ma_sound* s)
{
    if (s)
        ma_sound_stop(s);
}

void AudioSystem::resumeAudio(ma_sound* s)
{
    playAudio(s);
}
#pragma endregion

#pragma region 2D CHANGES
void AudioSystem::setLooping(ma_sound* s, bool loop)
{
    if (s)
        ma_sound_set_looping(s, loop);
}

void AudioSystem::setVolume(ma_sound* s, float vol)
{
    if (s)
        ma_sound_set_volume(s, vol);
}

void AudioSystem::setGroupVolume(HUGO::AudioTypes type, float volume)
{
}

void AudioSystem::setPitch(ma_sound* s, float pitch)
{
    if (s)
        ma_sound_set_pitch(s, pitch);
}

void AudioSystem::setAudioPosition(ma_sound* s, double seconds)
{
    if (!s)
        return;

    if (seconds < 0.0)
        seconds = 0.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    ma_uint64 frame = static_cast<ma_uint64>(seconds * sr);

    ma_uint64 totalFrames = 0;

    if (ma_sound_get_length_in_pcm_frames(s, &totalFrames) == MA_SUCCESS &&
        frame > totalFrames)
    {
        frame = totalFrames;
    }

    ma_sound_seek_to_pcm_frame(s, frame);
}

double AudioSystem::getAudioPosition(ma_sound* s)
{
    if (!s)
        return -1.0;

    ma_uint64 cursor = 0;

    if (ma_sound_get_cursor_in_pcm_frames(s, &cursor) != MA_SUCCESS)
        return -1.0;

    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());

    return static_cast<double>(cursor) / sr;
}
#pragma endregion

#pragma region 3D CHANGES
void AudioSystem::set3DPosition(ma_sound* s, const HUGO::Vector3& p)
{
    if (s)
        ma_sound_set_position(s, p.x, p.y, p.z);
}

void AudioSystem::setVelocity(ma_sound* s, const HUGO::Vector3& v)
{
    if (s)
        ma_sound_set_velocity(s, v.x, v.y, v.z);
}

void AudioSystem::setDirection(ma_sound* s, const HUGO::Vector3& d)
{
    if (s)
        ma_sound_set_direction(s, d.x, d.y, d.z);
}

void AudioSystem::setDopplerFactor(ma_sound* s, float factor)
{
    if (s)
        ma_sound_set_doppler_factor(s, factor);
}

void AudioSystem::set3DMinDistance(ma_sound* s, float distance)
{
    if (s)
        ma_sound_set_min_distance(s, distance);
}

void AudioSystem::set3DMaxDistance(ma_sound* s, float distance)
{
    if (s)
        ma_sound_set_max_distance(s, distance);
}

void AudioSystem::set3DRollof(ma_sound* s, float rolloff)
{
    if (s)
        ma_sound_set_rolloff(s, rolloff);
}

void AudioSystem::set3DCone(ma_sound* s, float innerAngle, float outerAngle, float outerGain)
{
    if (s)
        ma_sound_set_cone(s, innerAngle, outerAngle, outerGain);
}
#pragma endregion

#pragma region synchronization
void AudioSystem::updateListeners(const AudioListenerComponent& a, const TransformComponent& tr)
{
    const HUGO::Vector3& forward = tr.rotation.forward();
    const HUGO::Vector3& up = tr.rotation.up();

    ma_engine_listener_set_position(engine.get(), 0, tr.position.x, tr.position.y, tr.position.z);

    ma_engine_listener_set_direction(engine.get(), 0, forward.x, forward.y,forward.z);

    ma_engine_listener_set_world_up(engine.get(), 0, up.x, up.y, up.z);

    ma_engine_listener_set_velocity(engine.get(), 0, a.listenerVelocity.x, a.listenerVelocity.y,a.listenerVelocity.z);
}

void AudioSystem::processChanges(AudioComponent &c, const AudioDirtyTag &t)
{
    // get the pointer to the currently active voice (if it has)
    ma_sound* s = acquireVoice(c);

    if (t.flags  & HUGO::LOOP)
        setLooping(s, c.loop);

    if (t.flags & HUGO::VOLUME)
        setVolume(s, c.volume);

    if (t.flags & HUGO::PITCH)
        setPitch(s, c.pitch);

    if (t.flags & HUGO::STARTTIME)
        setAudioPosition(s, c.startTime);
}

void AudioSystem::process3DChanges(AudioComponent& a, const Audio3DInfoComponent &c, const TransformComponent& tr, const Audio3DDirtyTag& t)
{
    ma_sound* s = acquireVoice(a);

    if (t.flags & HUGO::Audio3DDirtyFlags::MINDISTANCE)
        set3DMinDistance(s, c.minDistance);

    if (t.flags & HUGO::Audio3DDirtyFlags::MAXDISTANCE)
        set3DMaxDistance(s, c.maxDistance);

    if (t.flags & HUGO::Audio3DDirtyFlags::VELOCITY)
        setVelocity(s, c.velocity);

    if (t.flags & HUGO::Audio3DDirtyFlags::DIRECTION)
        setDirection(s, c.direction);

    if (t.flags & HUGO::Audio3DDirtyFlags::DOPPLER)
        setDopplerFactor(s, c.dopplerFactor);

    if (t.flags & HUGO::Audio3DDirtyFlags::ROLLOF)
        set3DRollof(s, c.rolloff);

    if (t.flags & HUGO::Audio3DDirtyFlags::CONE)
        set3DCone(s, c.coneInnerAngle, c.coneOuterAngle, c.coneOuterGain);

    if (t.flags & HUGO::Audio3DDirtyFlags::MOVED){
        set3DPosition(s, tr.position);
    }
}
#pragma endregion

#pragma region HELPERS
ErrorOrVoid AudioSystem::loadAudiosFromFile(const char* fileName)
{
    // build full path using the system audio directory
    std::string fullPath = path + fileName;
    std::ifstream file(fullPath);

    if (!file.is_open())
    {
        StringError error;
        error.errorMessage = "Could not open audio file list";
        return ErrorOrVoid(error);
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::stringstream ss(line);

        std::string name;
        std::string filename;
        std::string typeStr;

        std::getline(ss, name, ';');
        std::getline(ss, filename, ';');
        std::getline(ss, typeStr, ';');

        HUGO::AudioTypes type =
            static_cast<HUGO::AudioTypes>(std::stoi(typeStr));

        HUGO::AudioID id = nextID++;

        audios.push_back({name, filename, type});
        nameToID[name] = id;

        pools.emplace_back();
    }

    return ErrorOrVoid();
}

ErrorOrVoid AudioSystem::addAudio(const char* name,
                                  HUGO::AudioTypes type)
{
    std::string audioName = name;

    if (nameToID.find(audioName) != nameToID.end())
    {
        StringError error;
        error.errorMessage = "Audio already registered";
        return ErrorOrVoid(error);
    }

    HUGO::AudioID id = nextID++;

    audios.push_back({audioName, audioName, type});
    nameToID[audioName] = id;

    pools.emplace_back();

    return ErrorOrVoid();
}
#pragma endregion

#pragma region UPDATES
void AudioSystem::update(double dt)
{
    elapsedTime += dt;

    // synk audio listeners
    for (auto& pack : scene->query<AudioListenerComponent,
                               TransformComponent,
                               TransformDirtyTag>())
    {
        AudioListenerComponent& listener = pack.get<AudioListenerComponent&>();
        TransformComponent& tr   = pack.get<TransformComponent&>();
        TransformDirtyTag& tag  = pack.get<TransformDirtyTag&>();
        updateListeners(listener, tr);
        scene->removeComponent<TransformDirtyTag>(tag.entity);
    }
    // 3D changes
    for (auto& pack :
         scene->query<AudioComponent, Audio3DInfoComponent,
                    TransformComponent, Audio3DDirtyTag>())
    {
        AudioComponent& a = pack.get<AudioComponent&>();
        Audio3DInfoComponent& info = pack.get<Audio3DInfoComponent&>();
        TransformComponent& tr   = pack.get<TransformComponent&>();
        Audio3DDirtyTag& tag  = pack.get<Audio3DDirtyTag&>();

        process3DChanges(a, info, tr, tag);

        scene->removeComponent<Audio3DDirtyTag>(tag.entity);
    }

    // general changes
    for (auto& pack :
         scene->query<AudioComponent, AudioDirtyTag>())
    {
        AudioComponent& c   = pack.get<AudioComponent&>();
        AudioDirtyTag& tag = pack.get<AudioDirtyTag&>();

        processChanges(c, tag);

        scene->removeComponent<AudioDirtyTag>(c.entity);
    }

    // play events
    for (auto& pack :
         scene->query<AudioComponent, AudioPlayTag>())
    {
        AudioComponent& c = pack.get<AudioComponent&>();

        ma_sound* s = acquireVoice(c);
        playAudio(s);

        c.isPlaying = true;

        scene->removeComponent<AudioPlayTag>(c.entity);
    }

    // stop events
    for (auto& pack :scene->query<AudioComponent, AudioStopTag>())
    {
        AudioComponent& c = pack.get<AudioComponent&>();

        if (c.activePtr)
        {
            stopAudio(c.activePtr);
            c.activePtr = nullptr;
        }

        c.isPlaying = false;
        scene->removeComponent<AudioStopTag>(c.entity);
    }

    for (auto& pack : scene->query<AudioComponent>())
    {
        AudioComponent& c = pack.get<AudioComponent&>();

        if (c.activePtr && !ma_sound_is_playing(c.activePtr))
        {
            c.activePtr = nullptr;
            c.isPlaying = false;
        }
    }
    // TEST STATE MACHINE
    int i = 0;

    for (auto& pack : scene->query<AudioComponent>())
    {
        AudioComponent& sc = pack.get<AudioComponent&>();
        EntityID id = sc.entity;

        if (i > 0)
        {
            ++i;
            continue;
        }

        // -------- STATE 1 : STOP --------
        if (!stopTriggered && elapsedTime >= x)
        {
            sc.isPlaying = false;
            scene->addComponent<AudioStopTag>(id);

            stopTriggered = true;
        }

        // -------- STATE 2 : RESTART --------
        if (!startTriggered && elapsedTime >= 2.0 * x)
        {
            sc.isPlaying = true;
            sc.pitch = 0.75f;
            sc.startTime = 10.0;

            AudioDirtyTag& tag = scene->addAndGetComponent<AudioDirtyTag>(id);
            tag.flags = static_cast<HUGO::AudioDirtyFlags>(HUGO::PITCH | HUGO::STARTTIME);
            scene->addComponent<AudioPlayTag>(id);

            startTriggered = true;
        }

        ++i;
    }
}

void AudioSystem::fixedUpdate(double dt)
{
}
#pragma endregion

void AudioSystem::createTestEntity()
{
    EntityID ent = scene->createEntity();
    AudioComponent& audio = scene->addAndGetComponent<AudioComponent>(ent);
    audio.type = HUGO::MUSIC;
    audio.audioId = nameToID["bgm"];
    
    scene->addComponent<AudioPlayTag>(ent);
    int i = 0;
    for (auto& pack : scene->query<AudioComponent>()) {
        AudioComponent& sc = pack.get<AudioComponent&>();
        if (i == 0){
            sc.isPlaying = true;
            sc.loop = false;
            sc.pitch = 2.0f;
            sc.startTime = 30.0;
            sc.volume = 0.1f;
            AudioDirtyTag& tag = scene->addAndGetComponent<AudioDirtyTag>(ent);
            tag.flags = static_cast<HUGO::AudioDirtyFlags>(
                            HUGO::AudioDirtyFlags::LOOP |
                            HUGO::AudioDirtyFlags::PITCH |
                            HUGO::AudioDirtyFlags::STARTTIME |
                            HUGO::AudioDirtyFlags::VOLUME);
            scene->addComponent<AudioPlayTag>(ent);
        }
    }
}
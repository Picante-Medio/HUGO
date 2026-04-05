#include "AudioSystem.h"
#include "AudioComponent.h"
#include "AudioListenerComponent.h"
#include "Audio3DInfoComponent.h"
#include "HugoError.h"
#include "HugoTypes.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "LuaBinder.h"

#include "MiniaudioModule.h"
#include "AudioAssetsModule.h"

#include <fstream>
#include <memory>
#include <sstream>

ErrorOrType<AudioInfo> AudioSystem::createVoice(HUGO::AudioID id, HUGO::AudioTypes type)
{
    StringError e;
    AudioAssetInfo* asset = AudioAssetsModule::Instance().getAsset(id);
    
    if (!asset) {       
        e.errorMessage = "[AudioSystem]: Audio ID not found";
        return ErrorOrType<AudioInfo>(e);
    }
    
    AudioInfo info;
    info.ptr = std::make_shared<Audio>();
    
    if (!info.ptr->init(engine.get(), asset->fullPath.c_str(), 0))
    {
        e.errorMessage = "[AudioSystem] Failed loading audio: " + asset->fullPath;
        return ErrorOrType<AudioInfo>(e);
    }
    
    // configuration
    if (type == HUGO::SOUND3D)
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

void AudioSystem::ensurePoolSize()
{
    size_t assetCount = AudioAssetsModule::Instance().size();
    if (pools.size() < assetCount) {
        pools.resize(assetCount);
    }
}

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
ma_sound* AudioSystem::getSound(HUGO::AudioID id, HUGO::AudioTypes type)
{
    AudioPool& pool = pools[id];
    
    // STEP 1: reuse
    for (auto& voice : pool.voices)
    {
        if (!voice.inUse)
        {
            voice.inUse = true;
            ma_sound_seek_to_pcm_frame(voice.ptr->get(), 0);
            return voice.ptr->get();
        }
    }
    
    // STEP 2: create new
    if (pool.voices.size() < static_cast<size_t>(queueSize))
    {   
        ErrorOrType<AudioInfo> result = createVoice(id, type);
        
        if (result.isError())
            return nullptr;
        
        pool.voices.emplace_back(std::move(result.getValue()));
        
        AudioInfo& v = pool.voices.back();
        v.inUse = true;
        
        ma_sound_set_end_callback(
            v.ptr->get(),
            AudioSystem::endCallback,
            &v);
        
        return v.ptr->get();
    }
    
    // STEP 3: steal
    AudioInfo& stolen = pool.voices[pool.stealIndex];
    pool.stealIndex = (pool.stealIndex + 1) % pool.voices.size();
    
    ma_sound_stop(stolen.ptr->get());
    ma_sound_seek_to_pcm_frame(stolen.ptr->get(), 0);
    
    stolen.inUse = true;
    
    // reconfig using the type
    if (type == HUGO::SOUND3D) {
        ma_sound_set_spatialization_enabled(stolen.ptr->get(), MA_TRUE);
        ma_sound_set_attenuation_model(stolen.ptr->get(), ma_attenuation_model_inverse);
    } else {
        ma_sound_set_spatialization_enabled(stolen.ptr->get(), MA_FALSE);
    }
    
    return stolen.ptr->get();
}

void AudioSystem::acquireVoice(AudioComponent &c)
{
    if (c.activePtr == nullptr) {
        c.activePtr = getSound(c.audioId, c.type);
    }
}

void AudioSystem::endCallback(void* data, ma_sound*)
{
    if (!data) return;
    AudioInfo& voice = *reinterpret_cast<AudioInfo*>(data);
    voice.inUse = false;
}

#pragma region PLAYBACK CONTROLS
void AudioSystem::playAudio(ma_sound* s)
{
    ma_sound_start(s);
}

void AudioSystem::playAudio(HUGO::AudioID id, HUGO::AudioTypes type)
{
    ma_sound* s = getSound(id, type);
    playAudio(s);
}

void AudioSystem::playAudioLUA(int id, int type) {
    if (type >= HUGO::AudioTypes::LAST_TYPE){
        return;
    }
    playAudio(static_cast<HUGO::AudioID>(id), static_cast<HUGO::AudioTypes>(type));
}

void AudioSystem::stopAudio(ma_sound* s)
{
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
    ma_sound_set_looping(s, loop);
}

void AudioSystem::setVolume(ma_sound* s, HUGO::Float vol)
{
    ma_sound_set_volume(s, vol);
}

void AudioSystem::setGroupVolume(HUGO::AudioTypes type, HUGO::Float volume)
{
    if (type == HUGO::AudioTypes::MUSIC)
        musicsGroupVolume = volume;
    else   
        soundsGroupVolume = volume;
}

void AudioSystem::setPitch(ma_sound* s, HUGO::Float pitch)
{
    ma_sound_set_pitch(s, pitch);
}

void AudioSystem::setAudioPosition(ma_sound* s, double seconds)
{   
    if (seconds < 0.0) seconds = 0.0;
    
    ma_uint32 sr = ma_engine_get_sample_rate(engine.get());
    ma_uint64 frame = static_cast<ma_uint64>(seconds * sr);
    
    ma_uint64 totalFrames = 0;
    if (ma_sound_get_length_in_pcm_frames(s, &totalFrames) == MA_SUCCESS && frame > totalFrames)
    {
        frame = totalFrames;
    }
    
    ma_sound_seek_to_pcm_frame(s, frame);
}

double AudioSystem::getAudioPosition(ma_sound* s)
{
    if (!s) return -1.0;
    
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

void AudioSystem::setDopplerFactor(ma_sound* s, HUGO::Float factor)
{
    ma_sound_set_doppler_factor(s, factor);
}

void AudioSystem::set3DMinDistance(ma_sound* s, HUGO::Float distance)
{
    ma_sound_set_min_distance(s, distance);
}

void AudioSystem::set3DMaxDistance(ma_sound* s, HUGO::Float distance)
{
    ma_sound_set_max_distance(s, distance);
}

void AudioSystem::set3DRollof(ma_sound* s, HUGO::Float rolloff)
{
    ma_sound_set_rolloff(s, rolloff);
}

void AudioSystem::set3DCone(ma_sound* s, HUGO::Float innerAngle, HUGO::Float outerAngle, HUGO::Float outerGain)
{
    ma_sound_set_cone(s, innerAngle, outerAngle, outerGain);
}
#pragma endregion

#pragma region synchronization
void AudioSystem::updateListeners(const AudioListenerComponent& a, const TransformComponent& tr)
{
    const HUGO::Vector3& forward = tr.getRotation().forward();
    const HUGO::Vector3& up = tr.getRotation().up();

    ma_engine_listener_set_position(engine.get(), 0, tr.getPosition().x, tr.getPosition().y, tr.getPosition().z);

    ma_engine_listener_set_direction(engine.get(), 0, forward.x, forward.y,forward.z);

    ma_engine_listener_set_world_up(engine.get(), 0, up.x, up.y, up.z);
    ma_engine_listener_set_velocity(engine.get(), 0, a.listenerVelocity.x, a.listenerVelocity.y, a.listenerVelocity.z);
}

void AudioSystem::processChanges(AudioComponent &c)
{
    setLooping(c.activePtr, c.loop);
    HUGO::Float mult = c.type != HUGO::AudioTypes::MUSIC ? soundsGroupVolume : musicsGroupVolume;
    setVolume(c.activePtr, c.volume*mult);
    setPitch(c.activePtr, c.pitch);
    if (c.startTime >= 0.0){
        setAudioPosition(c.activePtr, c.startTime);
        c.startTime = -1.0;
    }

    if (c.isPlaying && !ma_sound_is_playing(c.activePtr)){
        playAudio(c.activePtr);
    }
    else if (!c.isPlaying && ma_sound_is_playing(c.activePtr)){
        stopAudio(c.activePtr);
    }
}

void AudioSystem::process3DChanges(AudioComponent& a, const Audio3DInfoComponent &c, const TransformComponent& tr)
{
    set3DMinDistance(a.activePtr, c.minDistance);
    set3DMaxDistance(a.activePtr, c.maxDistance);
    setVelocity(a.activePtr, c.velocity); 
    setDirection(a.activePtr, c.direction);
    setDopplerFactor(a.activePtr, c.dopplerFactor);
    set3DRollof(a.activePtr, c.rolloff);
    set3DCone(a.activePtr, c.coneInnerAngle, c.coneOuterAngle, c.coneOuterGain);
    set3DPosition(a.activePtr, tr.getPosition());
}
#pragma endregion

#pragma region UPDATES
void AudioSystem::update(double)
{
    // sync audio listeners
    for (auto& pack : scene->query<AudioListenerComponent, TransformComponent>())
    {
        AudioListenerComponent& listener = pack.getDeprecated<AudioListenerComponent&>();
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        updateListeners(listener, tr);
    }
    
    // 3D changes
    for (auto& pack: scene->query<AudioComponent, Audio3DInfoComponent, TransformComponent>())
    {
        AudioComponent& a = pack.getDeprecated<AudioComponent&>();
        Audio3DInfoComponent& info = pack.getDeprecated<Audio3DInfoComponent&>();
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        process3DChanges(a, info, tr);
    }
    
    // general changes
    for (auto& pack: scene->query<AudioComponent>())
    {
        AudioComponent& c = pack.getDeprecated<AudioComponent&>();
        processChanges(c);
    }
    
    // cleanup finished voices
    for (auto& pack : scene->query<AudioComponent>())
    {
        AudioComponent& c = pack.getDeprecated<AudioComponent&>();
        if (c.activePtr && !ma_sound_is_playing(c.activePtr))
        {
            c.activePtr = nullptr;
            c.isPlaying = false;
        }
    }
}

void AudioSystem::fixedUpdate(double)
{
}

void AudioSystem::onLoad(){
    std::shared_ptr<LuaBinder> luaBinder = LuaBinder::getInstance();
    luaBinder->registerMethod<AudioSystem, void, int, int>(*this, &AudioSystem::playAudioLUA, "playAudio");
}

void AudioSystem::init(){
    engine = MiniaudioModule::Instance().getMiniaudioEngine();
    
    // Ensure pools are sized correctly
    ensurePoolSize();
}
#pragma endregion

#pragma region events
void AudioSystem::onAddAudioComponent(AudioComponent &c)
{
    acquireVoice(c);
}
#pragma endregion

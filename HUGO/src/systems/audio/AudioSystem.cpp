#include "AudioSystem.h"
#include "AudioComponent.h"
#include "AudioListenerComponent.h"
#include "Audio3DInfoComponent.h"
#include "HugoError.h"
#include "HugoTypes.h"
#include "TransformComponent.h"
#include "WorldTransformComponent.h"
#include "LuaBinder.h"

#include "MiniaudioModule.h"
#include "AudioAssetsModule.h"

#include <fstream>
#include <memory>
#include <sstream>

AudioSystem::~AudioSystem() {

    // Duplicated code same as of onExit(), intentional
    // in case onExit stores data of playing timestamp to resume

    for (auto& [entity, v] : voices) {
        ma_sound_stop(&v.sound);
        ma_sound_uninit(&v.sound);

        v.initialized = false;
        v.started = false;
    }

    voices.clear();
}

void 
AudioSystem::init() {
    engine = &MiniaudioModule::instance().getMiniaudioEngine();

    scene.onAddComponent<AudioComponent>().connect(this, 
        &AudioSystem::onAddAudioComponent);

    scene.onUpdateComponent<AudioComponent>().connect(this, 
        &AudioSystem::onUpdateAudioComponent);

    scene.onRemoveComponent<AudioComponent>().connect(this, 
        &AudioSystem::onRemoveAudioComponent);


    scene.onAddComponent<Audio3DInfoComponent>().connect(this, 
        &AudioSystem::onAddAudio3DInfoComponent);
    scene.onUpdateComponent<Audio3DInfoComponent>().connect(this, 
        &AudioSystem::onUpdateAudio3DInfoComponent);
    scene.onRemoveComponent<Audio3DInfoComponent>().connect(this, 
        &AudioSystem::onRemoveAudio3DInfoComponent);
}

void
AudioSystem::onLoad() {
    for (auto& c : scene.query<AudioComponent>())
    {
        c.modify<AudioComponent>([this](AudioComponent& audioCmp){
            onAddAudioComponent(audioCmp);
        });
    }
}

void 
AudioSystem::onExit()
{
    // Clear all sounds, they will not be in use and sounds
    // are referent to an audiocomponent, with the component
    // we can remake it so we free the memory.
    //
    // If we used miniaudio resource manager it would be less
    // important.
    for (auto& [entity, v] : voices) {
        ma_sound_stop(&v.sound);
        ma_sound_uninit(&v.sound);

        v.initialized = false;
        v.started = false;
    }

    voices.clear();
}

void 
AudioSystem::update(uint32_t){
    for (auto& pack : scene.query<AudioComponent>()){
        pack.modify<AudioComponent>([this](AudioComponent& a){
            AudioVoice& v = *getVoice(a.entity); 
            bool miniaudioPlaying = ma_sound_is_playing(&v.sound);
            // Natural end of play
            if (v.started && a.isPlaying && !miniaudioPlaying && !a.loop) {
                a.isPlaying = false;
                v.started = false;
            }
        });
    }
}

void
AudioSystem::fixedUpdate(uint32_t) {

    // Update 3d audio
    for (auto& q : scene.query<AudioComponent, Audio3DInfoComponent, WorldTransformComponent>()) {

        const WorldTransformComponent& tr = q.getConst<WorldTransformComponent>();

        auto it = voices.find(tr.entity);
        if (it == voices.end() || !it->second.initialized)
            continue;

        ma_sound_set_position(&(it->second.sound),
            tr.worldPosition.x,
            tr.worldPosition.y,
            tr.worldPosition.z);
    }

    auto listenersQuery = scene.query<AudioListenerComponent, WorldTransformComponent>();

    if (listenersQuery.size() > 0) {

        // First listener only, if there are more, well... skill issue I guess
        HUGO::ComponentTuple<AudioListenerComponent, WorldTransformComponent>& firstListener = 
            *listenersQuery.begin();

        const WorldTransformComponent& tr = firstListener.getConst<WorldTransformComponent>();

        ma_engine_listener_set_position(engine, 0,
            tr.worldPosition.x,
            tr.worldPosition.y,
            tr.worldPosition.z);

        HUGO::Vector3 f = tr.worldRotation.forward();

        ma_engine_listener_set_direction(engine, 0,
            f.x,
            f.y,
            f.z);

        ma_engine_listener_set_world_up(engine, 0,
            tr.worldRotation.up().x,
            tr.worldRotation.up().y,
            tr.worldRotation.up().z);
    }
}

AudioSystem::AudioVoice* 
AudioSystem::getVoice(EntityID e) {
    auto it = voices.find(e);

    if (it == voices.end())
        return nullptr;

    return &it->second;
}

void
AudioSystem::tryCreateSound(AudioAssetInfo *asset, AudioVoice& voice, bool stream) {
    if (
        ma_sound_init_from_file(
            engine,
            asset->fullPath.c_str(),
            0,
            nullptr,
            nullptr,
            &voice.sound
        ) == MA_SUCCESS)
    {
        voice.initialized = true;
        voice.id = asset->id;
    }
    else {
        voice.id = -1;
        voice.initialized = false;
    }
}

void 
AudioSystem::onAddAudioComponent(AudioComponent& audioCmp) {

    AudioAssetInfo* asset =
        AudioAssetsModule::instance().getAsset(audioCmp.audioName);

    AudioVoice& v = voices[audioCmp.entity];

    if (!asset)
        return;

    // This already exist, get out
    if (v.initialized) return;

    tryCreateSound(asset, v, audioCmp.streaming);

    if (scene.hasComponent<Audio3DInfoComponent>(audioCmp.entity)) {
        
        ma_sound_set_spatialization_enabled(&v.sound, MA_TRUE);

        if (v.initialized) {

            const Audio3DInfoComponent& audio3dCmp = 
                scene.getComponentConst<Audio3DInfoComponent>(audioCmp.entity);

            // Enable 3D audio
            ma_sound_set_spatialization_enabled(&v.sound, MA_TRUE);
            ma_sound_set_attenuation_model(&v.sound, ma_attenuation_model_linear);

            sync3D(audio3dCmp, v);
        }
    }
    else {
        ma_sound_set_spatialization_enabled(&v.sound, MA_FALSE);
    }

    sync(audioCmp, v);
}

void AudioSystem::onUpdateAudioComponent(AudioComponent& audioCmp) {

    AudioVoice *v = getVoice(audioCmp.entity);

    AudioAssetInfo* asset =
        AudioAssetsModule::instance().getAsset(audioCmp.audioName);

    // Id is diferent, remake audio
    if (asset->id != v->id) {
        ma_sound_stop(&v->sound);
        ma_sound_uninit(&v->sound);
        tryCreateSound(asset, *v);
    }

    if (!v->initialized)
        return;

    sync(audioCmp, *v);
}

void AudioSystem::onRemoveAudioComponent(AudioComponent& audioCmp) {

    AudioVoice *v = getVoice(audioCmp.entity);

    if (v->initialized) {
        ma_sound_stop(&v->sound);
        ma_sound_uninit(&v->sound);
        voices.erase(audioCmp.entity);
    }
}

void 
AudioSystem::onAddAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp) {

    AudioVoice *v = getVoice(audio3dCmp.entity);

    if (v == nullptr || !v->initialized)
        return;

    ma_sound *sound = &v->sound;

    // Enable 3D audio
    ma_sound_set_spatialization_enabled(sound, MA_TRUE);

    sync3D(audio3dCmp, *v);
}

void 
AudioSystem::onUpdateAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp) {

    AudioVoice *v = getVoice(audio3dCmp.entity);

    if (v == nullptr || !v->initialized)
        return;

    sync3D(audio3dCmp, *v);
}

void 
AudioSystem::onRemoveAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp) {
 
    AudioVoice *v = getVoice(audio3dCmp.entity);

    if (v == nullptr || !v->initialized)
        return;

    // Back to 2D, return to monkey
    ma_sound_set_spatialization_enabled(&v->sound, MA_FALSE);
}


void 
AudioSystem::sync(AudioComponent& audioCmp, AudioVoice& voice)
{
    if (!voice.initialized)
        return;

    ma_sound_set_volume(&voice.sound, audioCmp.volume);
    ma_sound_set_pitch(&voice.sound, audioCmp.pitch);
    ma_sound_set_looping(&voice.sound, audioCmp.loop);

    bool playing = ma_sound_is_playing(&voice.sound);

    // Natural end of play
    if (voice.started && audioCmp.isPlaying && !playing && !audioCmp.loop) {
        audioCmp.isPlaying = false;
        voice.started = false;
    }

    // Manual start
    if (audioCmp.isPlaying && !playing) {
        ma_sound_start(&voice.sound);
        voice.started = true;
    }
    // Manual stop
    else if (!audioCmp.isPlaying && playing) {
        ma_sound_stop(&voice.sound);
        voice.started = false;
    }
}

void
AudioSystem::sync3D(const Audio3DInfoComponent& audio3dCmp, AudioVoice& voice) {

    if (!voice.initialized)
        return;
    
    ma_sound_set_min_distance(&voice.sound, audio3dCmp.minDistance);
    ma_sound_set_max_distance(&voice.sound, audio3dCmp.maxDistance);
    ma_sound_set_rolloff(&voice.sound, audio3dCmp.rolloff);
}
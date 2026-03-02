#pragma once

#include "Audio.h"
#include "AudioTags.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "System.h"

#include <functional>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

// forward declarations
struct AudioComponent;
struct Audio3DInfoComponet;
struct TransformComponent;

/* ====================
   runtime voice data
   ====================*/

struct AudioInfo {
    std::unique_ptr<Audio> ptr;
    bool inUse = false;
};

struct AudioEntry {
    std::string name;
    std::string filename;
    HUGO::AudioTypes type;
};

struct AudioPool {
    std::vector<AudioInfo> voices;
    size_t stealIndex = 0; // round-robin stealing
};


/* ===========
   audio System
   ===========*/

class AudioSystem : public System
{
private:
    std::shared_ptr<ma_engine> engine;
    std::string path;

    int queueSize;
    HUGO::AudioID nextID = 0;

    float soundsGroupVolume = 1.0f;
    float musicsGroupVolume = 1.0f;

    /* ==========
       audio TADS
       ==========*/
    std::vector<AudioEntry> audios; // metadata
    std::unordered_map<std::string, HUGO::AudioID> nameToID; 
    std::vector<AudioPool> pools; // runtime voices

    /* =================
       PRIVATE FUNCTIONS
       =================*/

    // create a ma_sound ptr
    AudioInfo createVoice(HUGO::AudioID id);

    // acquire reusable voice (reuse/create/steal)
    ma_sound* getSound(HUGO::AudioID id);

    // check 3D usage
    ma_sound* get3DSound(HUGO::AudioID id);

    // get the sound already owned by the component
    ma_sound* getExistingSound(AudioComponent& c);
    // obtains a playable sound for the component, 
    // reusing or acquiring a new one from the pool
    ma_sound* acquireVoice(AudioComponent& c);

    /* ==============================================
       functions to process changes in some component 
       and update them with the audios
       ============================================== */

    void processChanges(AudioComponent& c, const AudioDirtyTag& t);
    void process3DChanges(const Audio3DInfoComponet& c,
                          const TransformComponent& tr,
                          const Audio3DDirtyTag& t);

    /* ================
       playback control
       ================ */

    void playAudio(ma_sound* s);
    void playAudio(HUGO::AudioID id);

    void stopAudio(ma_sound* s);
    void resumeAudio(ma_sound* s);

    /* ================
       controls 2D
       ================*/

    void setAudioPosition(ma_sound* s, double seconds);
    double getAudioPosition(ma_sound* s);

    void setLooping(ma_sound* s, bool loop);
    void setVolume(HUGO::AudioID id, float vol);
    void setGroupVolume(HUGO::AudioTypes type, float vol);
    void setPitch(ma_sound* s, float pitch);

    /* ===========
       controls 3D
       ===========*/

    void set3DPosition(ma_sound* s, const HUGO::Vector3& p);
    void setVelocity(ma_sound* s, const HUGO::Vector3& v);
    void setDirection(ma_sound* s, const HUGO::Vector3& d);

    void setDopplerFactor(ma_sound* s, float factor);
    void set3DMinDistance(ma_sound* s, float distance);
    void set3DMaxDistance(ma_sound* s, float distance);
    void set3DRollof(ma_sound* s, float rollof);
    void set3DCone(ma_sound* s,
                   float innerAngle,
                   float outerAngle,
                   float outerGain);

    
    // internal callback
    static void endCallback(void* data, ma_sound* pSound);

    /* ====================================
       ONLY FOR TESTING! IT WILL BE DELETED
       ==================================== */

    double x = 5000.0;
    double elapsedTime = 0.0;
    bool stopTriggered = false;
    bool startTriggered = false;

public:
    AudioSystem(std::shared_ptr<Scene> scene,
                std::shared_ptr<ma_engine> maEngine,
                const std::string& path = "test_media/",
                int queueSize = 10);

    virtual ~AudioSystem() = default;
    
    // Load assets
    ErrorOrVoid loadAudiosFromFile(const char* path);
    void update(double dt) override;
    void fixedUpdate(double dt) override;

    ErrorOrVoid addAudio(const char* name, HUGO::AudioTypes type);

    /* ====================================
       ONLY FOR TESTING! IT WILL BE DELETED
       ==================================== */

    void createTestEntity();
};
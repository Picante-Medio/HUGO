#pragma once

#include "Audio.h"
#include "AudioTags.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "System.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// forward declarations
struct AudioComponent;
struct AudioListenerComponent;
struct Audio3DInfoComponent;
struct TransformComponent;

/* ====================
   runtime voice data
   ====================*/
struct AudioInfo {
    std::shared_ptr<Audio> ptr;
    bool inUse = false;
};

struct AudioEntry {
    std::string name;
    std::string filename;
    HUGO::AudioTypes type;
};

struct AudioPool {
    std::vector<AudioInfo> voices;
    size_t stealIndex = 0; // round-robin voice stealing
};

class MiniaudioModule;


/* =============
   audio system
   ============*/
class AudioSystem : public System
{
private:
   std::shared_ptr<ma_engine> engine;
   std::string path;

   int queueSize;

   HUGO::AudioID nextID = 0;

   float soundsGroupVolume = 1.0f;
   float musicsGroupVolume = 1.0f;

   /* ===========
      audio TADS
      ===========*/

   std::vector<AudioEntry> audios; // metadata
   std::unordered_map<std::string, HUGO::AudioID> nameToID;
   std::vector<AudioPool> pools; // runtime voices

   /* =================
      PRIVATE FUNCTIONS
      =================*/

   // create a ma_sound ptr
   ErrorOrType<AudioInfo> createVoice(HUGO::AudioID id);

   // acquire reusable voice (reuse/create/steal)
   ma_sound* getSound(HUGO::AudioID id);

   // obtains a playable sound for the component, 
   // reusing or acquiring a new one from the pool
   ma_sound* acquireVoice(AudioComponent& c);


   /* ==============================================
      functions to process changes in some component 
      and update them with the audios
      ============================================== */

   void updateListeners(const AudioListenerComponent& a,
                        const TransformComponent& tr);

   void processChanges(AudioComponent& c,
                     const AudioDirtyTag& t);

   void process3DChanges(AudioComponent& a,
                        const Audio3DInfoComponent& c,
                        const TransformComponent& tr,
                        const Audio3DDirtyTag& t);

   /* ================
      playback control
      ================ */

   void playAudio(ma_sound* s);
   void playAudio(HUGO::AudioID id);
   void playAudioLUA(int id);

   void stopAudio(ma_sound* s);
   void resumeAudio(ma_sound* s);


   /* ================
      controls 2D
      ================*/

   void setAudioPosition(ma_sound* s, double seconds);
   double getAudioPosition(ma_sound* s);

   void setLooping(ma_sound* s, bool loop);
   void setVolume(ma_sound* s, float volume);
   void setGroupVolume(HUGO::AudioTypes type, float volume);
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
   void set3DRollof(ma_sound* s, float rolloff);

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
               std::shared_ptr<MiniaudioModule> miniaudioModule,
               const std::string& path = "test_media/",
               int queueSize = 10);

   virtual ~AudioSystem() = default;


   // load assets
   ErrorOrVoid loadAudiosFromFile(const char* fileName);
   ErrorOrVoid addAudio(const char* name, HUGO::AudioTypes type);

   // updates
   void update(double dt) override;
   void fixedUpdate(double dt) override;

   /* ====================================
      ONLY FOR TESTING! IT WILL BE DELETED
      ==================================== */
   void createTestEntity();
};
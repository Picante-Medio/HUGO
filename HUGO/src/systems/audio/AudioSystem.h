#pragma once

#include "Audio.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoSystemMacros.h"

#include <memory>
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

struct AudioPool {
    std::vector<AudioInfo> voices;
    size_t stealIndex = 0; // round-robin voice stealing
};

/* =============
   audio system
   ============*/
HSYSTEM(AudioSystem)
private:
   std::shared_ptr<ma_engine> engine;

   const HUGO::Int queueSize = 10;

   HUGO::Float soundsGroupVolume = 1.0f;
   HUGO::Float musicsGroupVolume = 1.0f;

   std::vector<AudioPool> pools; // runtime voices

   /* =================
      PRIVATE FUNCTIONS
      =================*/

   // create a ma_sound ptr
   ErrorOrType<AudioInfo> createVoice(HUGO::AudioID id, HUGO::AudioTypes type);

   // acquire reusable voice (reuse/create/steal)
   ma_sound* getSound(HUGO::AudioID id, HUGO::AudioTypes type);

   // obtains a playable sound for the component
   ma_sound* acquireVoice(AudioComponent& c);

   void ensurePoolSize();
   void prewarmVoices(size_t voicesPerSound = 2);

   /* ==============================================
      sync with ECS
      ============================================== */

   void updateListeners(const AudioListenerComponent& a,
                        const TransformComponent& tr);

   void processChanges(AudioComponent& c);

   void process3DChanges(AudioComponent& a, const Audio3DInfoComponent& c, const TransformComponent& tr);

   /* ================
      playback control
      ================ */

   void playAudio(ma_sound* s);
   void playAudio(HUGO::AudioID id, HUGO::AudioTypes type);
   void playAudioLUA(int id, int type);

   void stopAudio(ma_sound* s);
   void resumeAudio(ma_sound* s);

   /* ================
      controls 2D
      ================ */

   void setAudioPosition(ma_sound* s, double seconds);
   double getAudioPosition(ma_sound* s);

   void setLooping(ma_sound* s, bool loop);
   void setVolume(ma_sound* s, HUGO::Float volume);
   void setGroupVolume(HUGO::AudioTypes type, HUGO::Float volume);
   void setPitch(ma_sound* s, HUGO::Float pitch);

   /* ===========
      controls 3D
      =========== */

   void set3DPosition(ma_sound* s, const HUGO::Vector3& p);
   void setVelocity(ma_sound* s, const HUGO::Vector3& v);
   void setDirection(ma_sound* s, const HUGO::Vector3& d);

   void setDopplerFactor(ma_sound* s, HUGO::Float factor);
   void set3DMinDistance(ma_sound* s, HUGO::Float distance);
   void set3DMaxDistance(ma_sound* s, HUGO::Float distance);
   void set3DRollof(ma_sound* s, HUGO::Float rolloff);

   void set3DCone(ma_sound* s, HUGO::Float innerAngle, HUGO::Float outerAngle, HUGO::Float outerGain);

   // internal callback
   static void endCallback(void* data, ma_sound*);

public:

   virtual ~AudioSystem() = default;

   // updates
   void update(double) override;
   void fixedUpdate(double) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()

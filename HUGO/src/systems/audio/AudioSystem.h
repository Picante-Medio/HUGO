#pragma once

#include "Audio.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "System.h"

#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>

struct AudioInfo{
  HUGO::AudioTypes type;
  std::unique_ptr<Audio> ptr;
};

/*
Audios will be loaded from a lua script (hopefully)
*/

//forward declarations
struct AudioComponent;
struct Audio3DInfoComponet;
struct TransformComponent;

class AudioSystem : public System {
  std::shared_ptr<ma_engine> engine;
  //path where all the audio files are located 
  std::string path;

  std::vector<AudioInfo> audios;
    // format: path -> id
  // unordered map if we want to know its id using a string
  std::unordered_map<std::string, HUGO::AudioID> audioMap;
  std::unordered_map<std::string, std::queue<Audio>> audioMaps;
  HUGO::AudioID nextID;

  float soundsGroupVolume = 1.0f; // global volume controller for 2D and 3D sfx
  float musicsGroupVolume = 1.0f; // global volume controller for musics

  // private functions as helpers

  // private function to load and store all the audio files we are going to include
  void loadFromFile(const char* path);
  // private function to create a ma_sound using an audio file
  AudioInfo createAudio(const char *filename, HUGO::AudioTypes type);
  // private function to get a ma_sound pointer of the vector to do operations
  ma_sound *getSound(HUGO::AudioID id);
  // auxiliar method to check if audios[id] exists and its a 3d sound
  ma_sound *get3DSound(HUGO::AudioID id);

  //(TODO) sound queue
  // functions to process changes of one audio
  void processChanges(AudioComponent& c);
  void process3DChanges(Audio3DInfoComponet& c, const TransformComponent& t);

  // playback control functions
  void playAudio(ma_sound* s);
  void playAudio(HUGO::AudioID id);
  void stopAudio(ma_sound* s);
  void resumeAudio(ma_sound* s);

  // getters and setters functions
  void setAudioPosition(ma_sound* s,
                        double seconds); // set playback position in seconds
  double getAudioPosition(
      ma_sound* s); // get current playback position in seconds
  void setLooping(ma_sound* s,
                  bool loop); // enable or disable looping for the audio
  void setVolume(HUGO::AudioID id,
                  float vol); // set playback volume (0.0 = silent, 1.0 = full)
  void setPitch(ma_sound* s,  
                  float pitch);  // set pitch (1.0 = normal, 0.5 = lower, 2.0 = higher)
  void setGroupVolume(HUGO::AudioTypes type,
                      float volume); // set the same volume to the whole group

  // 3d audio functions
  void set3DPosition(ma_sound* s, const HUGO::Vector3& p);
  void setVelocity(ma_sound* s, const HUGO::Vector3& v); // set movement of sound in space (useful in doppler)
  void setDirection(ma_sound* s,const HUGO::Vector3& d); // set where the sound points (useful in cone)
  void setDopplerFactor(ma_sound* s, float factor);
  void set3DMinDistance(ma_sound* s, float distance);
  void set3DMaxDistance(ma_sound* s, float distance);
  void set3DRollof(ma_sound* s, float rollof); //Rolloff determines how the volume of a sound decreases as you move away from it in 3D space
  void set3DCone(ma_sound* s, float innerAngle, float outerAngle, float outerGain);
  
  // internal end callback
  static void endCallback(void* data, ma_sound* pSound);
  //only for testing!
  double x =5000.0;
  double elapsedTime = 0.0;
  bool stopTriggered = false;
  bool startTriggered=false;
  public:
  AudioSystem(std::shared_ptr<Scene> scene,
              std::shared_ptr<ma_engine> maEngine,
              const std::string& path = "test_media/");
  virtual ~AudioSystem() = default;

  void update(double dt) override;
  void fixedUpdate(double dt) override;
  // add an audio to the system
  // type determines if it's 2D, 3D or music
  void addAudio(const char *name, HUGO::AudioTypes type);

  void createTestEntity();
};

#pragma once

#include "Audio.h"
#include "HugoTypes.h"
#include "System.h"

#include <vector>
#include <string>
#include <unordered_map>

typedef struct {
  HUGO::AudioTypes type;
  std::unique_ptr<Audio> ptr;
} AudioInfo;

/*
Audios will be loaded from a lua script (hopefully)
*/
class AudioSystem : public System {
  std::shared_ptr<ma_engine> engine;
  std::vector<AudioInfo> audios;
    // format: path -> id
  // unordered map if we want to know its id using a string
  std::unordered_map<std::string, HUGO::AudioID> audioMap;
  
  HUGO::AudioID nextID;

  float soundsGroupVolume = 1.0f; // global volume controller for 2D and 3D sfx
  float musicsGroupVolume = 1.0f; // global volume controller for musics

  //path where all the audio files are located 
  std::string path = "test_media/";
  // private functions as helpers

  // private function to create a ma_sound using an audio file
  AudioInfo createAudio(const char *filename, HUGO::AudioTypes type);
  // private function to get a ma_sound pointer of the vector to do operations
  ma_sound *getSound(HUGO::AudioID id);
  // auxiliar method to check if audios[id] exists and its a 3d sound
  ma_sound *get3DSound(HUGO::AudioID id);


  //(TODO) sound queue

  void handleMessages();

  void loadFromFile(const char* path);

  public:
  AudioSystem(std::shared_ptr<Scene> scene,
              std::shared_ptr<ma_engine> maEngine);
  virtual ~AudioSystem() = default;

  void update(double dt) override;
  void fixedUpdate(double dt) override;
  // add an audio to the system
  // type determines if it's 2D, 3D or music
  void addAudio(const char *name, HUGO::AudioTypes type);

  // playback control functions
  void playAudio(HUGO::AudioID id);
  void stopAudio(HUGO::AudioID id);
  void resumeAudio(HUGO::AudioID id);

  // getters and setters functions
  void setAudioPosition(HUGO::AudioID id,
                        double seconds); // set playback position in seconds
  double getAudioPosition(
      HUGO::AudioID id); // get current playback position in seconds
  void setLooping(HUGO::AudioID id,
                  bool loop); // enable or disable looping for the audio
  void setVolume(HUGO::AudioID id,
                 float vol); // set playback volume (0.0 = silent, 1.0 = full)

  void setGroupVolume(HUGO::AudioTypes type,
                      float volume); // set the same volume to the whole group

  // 3d audio functions
  void set3DPosition(HUGO::AudioID id, float x, float y, float z);
  void setVelocity(HUGO::AudioID id, float vx, float vy, float vz);
  void setDirection(HUGO::AudioID id, float dx, float dy, float dz);
  void setDopplerFactor(HUGO::AudioID id, float factor);

  void createTestEntity();
};

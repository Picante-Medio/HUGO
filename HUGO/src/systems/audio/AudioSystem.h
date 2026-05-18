#pragma once

#include "Audio.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"
#include "HugoSystemMacros.h"

#include <unordered_map>
#include <memory>
#include <vector>

// forward declarations
struct AudioComponent;
struct AudioListenerComponent;
struct Audio3DInfoComponent;
struct WorldTransformComponent;

/// @brief Represents a sound that an entity emmits
struct AudioVoice
{
    ma_sound sound{};
    bool initialized = false;
	bool started = false;
};

HSYSTEM(AudioSystem)
private:

	/// @brief Engine reference, initialized in init
    std::shared_ptr<ma_engine> engine;

    /// @brief Entity
    std::unordered_map<EntityID, AudioVoice> voices;
	/// @brief 
	/// @param name 
	/// @return 
	AudioVoice* getVoice(EntityID e);

	void sync(AudioComponent& audioCmp, AudioVoice& voice);
	void sync3D(const Audio3DInfoComponent& audio3dCmp, AudioVoice& voice);

	// Events
	void onAddAudioComponent(AudioComponent& audioCmp);
	void onUpdateAudioComponent(AudioComponent& audioCmp);
	void onRemoveAudioComponent(AudioComponent& audioCmp);

	void onAddAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp);
	void onUpdateAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp);
	void onRemoveAudio3DInfoComponent(Audio3DInfoComponent& audio3dCmp);

public:
    virtual ~AudioSystem();

	// updates
	void update(uint32_t) override {}
	void fixedUpdate(uint32_t) override;

	void onLoad() override;
	void init() override;
	void onExit() override;
END_HSYSTEM()
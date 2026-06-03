#pragma once

#include "OgreModule.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"

#include <unordered_map>

struct ParticleEmitterComponent;
struct WorldTransformComponent;
HSYSTEM(ParticleSystem)
private:

    struct ParticleInstance {
        Ogre::ParticleSystem* ps = nullptr;
        Ogre::SceneNode* node = nullptr;
    };

    std::unordered_map<EntityID, ParticleInstance> activeParticleSystems;
    Ogre::SceneManager* sm = nullptr;

    void createParticleSystem(EntityID id, const std::string& particleName, ParticleEmitterComponent& pe, const HUGO::Vector3& iniPos);
    void destroyParticleSystem(EntityID id);
    void processChanges(const ParticleEmitterComponent& pe, const WorldTransformComponent& tr);
    void processPosition(const WorldTransformComponent &tr);
    // component events handlers
    void onAddParticleEmitterComponent(ParticleEmitterComponent& pe);
    void onUpdateParticleEmitterComponent(ParticleEmitterComponent& pe);
public:
    ~ParticleSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(uint32_t) override;
    void update(uint32_t) override;

END_HSYSTEM()
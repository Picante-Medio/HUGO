#pragma once

#include "OgreModule.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"

#include <unordered_map>

struct ParticleEmitterComponent;
struct TransformComponent;
HSYSTEM(ParticleSystem)
private:

    struct ParticleInstance {
        Ogre::ParticleSystem* ps = nullptr;
        Ogre::SceneNode* node = nullptr;
    };

    std::unordered_map<EntityID, ParticleInstance> activeParticleSystems;
    Ogre::SceneManager* sm = nullptr;

    void createParticleSystem(EntityID id, const HUGO::String& particleName, ParticleEmitterComponent& pe, const HUGO::Vector3& iniPos);
    void destroyParticleSystem(EntityID id);
    void processChanges(const ParticleEmitterComponent& pe, const TransformComponent& tr);

    // component events handlers
    void onAddParticleEmitterComponent(ParticleEmitterComponent& pe);
    void onUpdateParticleEmitterComponent(ParticleEmitterComponent& pe);
public:
    ~ParticleSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(double) override;
    void update(double) override;

END_HSYSTEM()
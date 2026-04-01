#pragma once

#include "OgreModule.h"
#include "HugoError.h"
#include "HugoMath.h"
#include "HugoSystemMacros.h"
#include "HugoTypes.h"

#include <unordered_map>

struct ParticleEmitterComponent;
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
    void processCreations();
    void processChanges();

public:
    ~ParticleSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(double) override;
    void update(double dt) override;

END_HSYSTEM()
#pragma once

#include "HugoSystemMacros.h"
#include "Ogre.h"
#include <unordered_map>

struct LightComponent;

HSYSTEM(LightSystem)

    // Ogre Light Data
    struct LightInfo {
        Ogre::Light* ogreLight = nullptr;  // Pointer to the Ogre light
        Ogre::SceneNode* node = nullptr;   // Scene node that controls light transform
    };

    Ogre::Root* ogreRoot;
    Ogre::SceneManager* sceneManager;

    std::unordered_map<EntityID, LightInfo> lightMap;

    void unregisterLightEntity(LightComponent& comp);
    void registerLightEntity(LightComponent& comp);

    void destroyLight(LightInfo& info);

    void onUpdateLightComponent(LightComponent& light);

    void setAmbientLight(HUGO::Float r, HUGO::Float g, HUGO::Float b);

public:
    void init() override;
    void onLoad() override;
    void fixedUpdate(uint32_t) override {}
    void update(uint32_t) override;
    ~LightSystem();

END_HSYSTEM()
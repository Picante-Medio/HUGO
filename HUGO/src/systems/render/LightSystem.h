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
    ErrorOrVoid createOgreLight(EntityID entity);

    void onUpdateLightComponent(LightComponent& light);

public:
    void init() override;
    void onLoad() override{}
    void fixedUpdate(double) override {}
    void update(double dt) override;
    ~LightSystem();

END_HSYSTEM()
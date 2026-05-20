#pragma once

#include "HugoSystemMacros.h"
#include "Ogre.h"
#include "LightSystem.h"

struct CameraComponent;

HSYSTEM(CameraSystem)

    // Ogre Camera Data
    struct CameraInfo {
        Ogre::Camera* ogreCamera = nullptr; // Pointer to the Ogre camera object
        Ogre::SceneNode* node = nullptr;    // Scene node that controls camera transform
    };

    Ogre::Root* ogreRoot;
    Ogre::SceneManager* sceneManager;

    std::unordered_map<EntityID, CameraInfo> cameraMap; 

    EntityID m_mainCamera = HUGO::INVALID_ENTITY;
    void setMainCamera(const CameraComponent& camComp);

    // Register and unregister for cameras created outside the RenderSystem
    void registerCameraEntity(CameraComponent& comp);
    void unregisterCameraEntity(CameraComponent& comp);
    void cameraChanged(CameraComponent& comp);

    void destroyCamera(EntityID entity, CameraInfo& camInfo);

public:
    void init() override;
    void onLoad() override;
    
void fixedUpdate(uint32_t) override {}
    void update(uint32_t) override;
    ~CameraSystem(); 

END_HSYSTEM()

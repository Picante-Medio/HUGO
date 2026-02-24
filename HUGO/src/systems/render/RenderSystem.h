#pragma once
#include <vector>
#include <unordered_map>
#include "System.h"
#include "HugoMath.h"
#include "Ogre.h"
#include "Ecs.h"

struct OgreObject{
    Ogre::Entity* ogreEntity;
    Ogre::SceneNode* sceneNode;
};

// For multiple cameras and viewports
struct CameraResources {
    Ogre::Camera* ogreCamera = nullptr;
    Ogre::SceneNode* node = nullptr;
};

struct ViewportResources {
    Ogre::Viewport* ogreViewport = nullptr;
    EntityID lastCameraEntity = 0;
};

class RenderSystem : public System{
    std::shared_ptr<Ogre::Root> ogreRoot;
    Ogre::SceneManager* sceneManager;
    Ogre::RenderWindow* renderWindow; 

    // Ogre::Viewport* viewport;
    // Ogre::SceneNode* defaultCameraNode;

    std::unordered_map<EntityID, CameraResources> cameraMap; // Provisional
    std::unordered_map<EntityID, ViewportResources> viewportMap;

    EntityID mainCameraEntity = 0;
    EntityID mainViewportEntity = 0;

    std::vector<std::pair<int, int>> resolutions;

    void updateViewportDimensions();

    void createOgreCamera(EntityID entity);
    void createOgreViewport(EntityID entity);
    // void destroyOgreCamera(EntityID entity);
    // void destroyOgreViewport(EntityID entity);


    void handleTag();
    OgreObject createOgreObject(EntityID entity, const std::string& meshName, const std::string& materialName="");

public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> root, Ogre::RenderWindow* window);
    void update(double dt) override;
    void fixedUpdate(double dt) override;

    void createTestEntity();
    
    // Creation of viewport and camera (helpers)
    EntityID createCameraEntity(const HUGO::Vector3& position = HUGO::Vector3(0, 0, 0));
    EntityID createViewportEntity();

    // Register and unregister for cameras and viewports created outside the RenderSystem
    void registerCameraEntity(EntityID entity);
    void registerViewportEntity(EntityID entity);
    
    void unregisterCameraEntity(EntityID entity);
    void unregisterViewportEntity(EntityID entity);

    // Window configuration
    void setfullScreen();
    void setWindowed(bool fullScreen);
    void resizeScreen(unsigned int widthPt, unsigned int heightPt);

    // Main camera and viewport configuration
    void setMainCamera(EntityID cameraEntity);
    EntityID getMainCamera() const;
    
    void setMainViewport(EntityID viewportEntity);
    EntityID getMainViewport() const;

    // Global light configuration
    void setAmbientLight(float r, float g, float b);
};

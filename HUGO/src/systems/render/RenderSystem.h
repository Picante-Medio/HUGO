#pragma once
#include <vector>
#include <unordered_map>

#include "System.h"
#include "Ecs.h"
#include "Ogre.h"
#include "PagedArray.h"
#include "HugoTypes.h"
#include "HugoMath.h"

struct RenderComponent;

struct OgreObject{
    HUGO::OgreObjectID id;
    Ogre::Entity* ogreEntity = nullptr;
    Ogre::SceneNode* ogreSceneNode = nullptr;
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
    Ogre::SceneManager* sceneManager = nullptr;
    Ogre::RenderWindow* renderWindow = nullptr; 

    std::unordered_map<EntityID, CameraResources> cameraMap; // Provisional
    std::unordered_map<EntityID, ViewportResources> viewportMap;

    std::vector<HUGO::OgreObjectID> freeOgreObjectId;
    std::vector<std::size_t> idToIndex;
    PagedArray<OgreObject, 128> ogreObjects;

    EntityID mainCameraEntity = 0;
    EntityID mainViewportEntity = 0;

    std::vector<std::pair<int, int>> resolutions;

    void updateViewportDimensions();

    void createOgreCamera(EntityID entity);
    void createOgreViewport(EntityID entity);
    // void destroyOgreCamera(EntityID entity);
    // void destroyOgreViewport(EntityID entity);

    void handleTag();

    //OgreObjects
    HUGO::OgreObjectID createOgreObject(const std::string& meshName, const std::string& materialName="");
    OgreObject& getOgreObject(HUGO::OgreObjectID id);
    void removeOgreObject(HUGO::OgreObjectID& id);
    void destroyOgreObject(OgreObject& obj);
      
public:
    RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> root, Ogre::RenderWindow* window);
    ~RenderSystem() override;
    
    void update(double dt) override;
    void fixedUpdate(double dt) override;

    void createTestEntity();
    // Materials
    void setMaterial(EntityID entity,const std::string& materialName);
    
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

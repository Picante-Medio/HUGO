#pragma once
#include <vector>
#include <unordered_map>

#include "System.h"
#include "Ecs.h"
#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include "PagedArray.h"
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoError.h"

struct RenderComponent;
class RenderSystem : public System{
    // Ogre Camera Information
    struct CameraInfo {
        Ogre::Camera* ogreCamera = nullptr;
        Ogre::SceneNode* node = nullptr;
    };

    // Ogre Viewport Information
    struct ViewportInfo {
        EntityID lastCameraEntity = 0;
        Ogre::Viewport* ogreViewport = nullptr;
    };

    // Ogre Light Information
    struct LightInfo {
        Ogre::Light* ogreLight = nullptr;
        Ogre::SceneNode* node = nullptr;
    };

    // OgreObject Information
    struct OgreObject{
        HUGO::OgreObjectID id;
        Ogre::Entity* ogreEntity = nullptr;
        Ogre::SceneNode* ogreSceneNode = nullptr;
    };
    
    Ogre::Root* ogreRoot;
    Ogre::SceneManager* sceneManager = nullptr;
    Ogre::RenderWindow* renderWindow = nullptr;

    std::unordered_map<EntityID, CameraInfo> cameraMap; // Provisional
    std::unordered_map<EntityID, ViewportInfo> viewportMap; // Provisional
    std::unordered_map<EntityID, LightInfo> lightMap; // Provisional

    std::vector<HUGO::OgreObjectID> freeOgreObjectId;
    std::vector<std::size_t> idToIndex;
    PagedArray<OgreObject, 128> ogreObjects;

    EntityID mainCameraEntity = 0;
    EntityID mainViewportEntity = 0;

    void updateViewportDimensions();

    ErrorOrVoid createOgreCamera(EntityID entity);
    ErrorOrVoid createOgreLight(EntityID entity);
    void createOgreViewport(EntityID entity);
    // void destroyOgreCamera(EntityID entity);
    // void destroyOgreViewport(EntityID entity);

    void handleTag();

    // Materials
    void setMaterial(EntityID entity,const std::string& materialName);   

    // Register and unregister for cameras and viewports created outside the RenderSystem
    void registerCameraEntity(EntityID entity);
    void registerViewportEntity(EntityID entity);
    void registerLightEntity(EntityID entity);
    
    void unregisterCameraEntity(EntityID entity);
    void unregisterViewportEntity(EntityID entity);
    void unregisterLightEntity(EntityID entity);

    //OgreObjects
    ErrorOrType<HUGO::OgreObjectID> createOgreObject(const std::string& meshName, const std::string& materialName="");
    OgreObject& getOgreObject(HUGO::OgreObjectID id);
    void removeOgreObject(HUGO::OgreObjectID& id);
    void destroyOgreObject(OgreObject& obj);


    // Creation of viewport and camera (helpers)
    EntityID createCameraEntity(const HUGO::Vector3& position = HUGO::Vector3(0, 0, 0));
    EntityID createViewportEntity();
    EntityID createLightEntity(const HUGO::Vector3& position = HUGO::Vector3(0, 0, 0)
                   , HUGO::Vector4 diff = HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f), HUGO::Vector4 specular= HUGO::Vector4(1.0f, 1.0f, 1.0f, 1.0f)
                   , HUGO::LightType lType = HUGO::LightType::LT_POINT
                   , const HUGO::Radian& outer = HUGO::Radian(0.0f), const HUGO::Radian& inner = HUGO::Radian(0.0f)
                   , float falloff = 1.0f, float nearClip = 1.0f, float shadowNearClipDist = 1.0f, float shadowFarClipDist = 100.0f);

                   
    // Global light configuration
    void setAmbientLight(float r, float g, float b);

    // Main camera and viewport configuration
    void setMainCamera(EntityID cameraEntity);
    
    void setMainViewport(EntityID viewportEntity);
      
public:
    RenderSystem(std::shared_ptr<Scene> scene, Ogre::RenderWindow* window,  std::shared_ptr<Ogre::OverlaySystem> overlay);
    ~RenderSystem() override;
    
    void fixedUpdate(double dt) override;
    void update(double dt) override;

    Ogre::SceneManager* getSM() const;
};

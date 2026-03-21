#pragma once
#include <vector>
#include <unordered_map>

#include "HugoSystemMacros.h"
#include "EcsTypes.h"
#include "Ogre.h"
#include "PagedArray.h"
#include "HugoTypes.h"
#include "HugoMath.h"
#include "HugoError.h"

struct RenderComponent;
HSYSTEM(RenderSystem)
    // Ogre Camera Data
    struct CameraInfo {
        Ogre::Camera* ogreCamera = nullptr; // Pointer to the Ogre camera object
        Ogre::SceneNode* node = nullptr;    // Scene node that controls camera transform
    };

    // Ogre Viewport Data
    struct ViewportInfo {
        EntityID lastCameraEntity = 0;
        Ogre::Viewport* ogreViewport = nullptr;  // Pointer to the Ogre viewport
    };

    // Ogre Light Data
    struct LightInfo {
        Ogre::Light* ogreLight = nullptr;  // Pointer to the Ogre light
        Ogre::SceneNode* node = nullptr;   // Scene node that controls light transform
    };

    // OgreObject Data
    // Represents an Ogre renderable entity.
    struct OgreObject{
        HUGO::OgreObjectID id = HUGO::INVALID_OGRE_OBJECT_ID; 
        Ogre::Entity* ogreEntity = nullptr;
        Ogre::SceneNode* ogreSceneNode = nullptr;   // Scene node for transform
        std::unordered_map<HUGO::String, Ogre::AnimationState*> animationStates; //Stored animation states
    };

    //Ogre References
    Ogre::Root* ogreRoot;
    Ogre::SceneManager* sceneManager = nullptr;

    //Mapping ECS entities with their corresponding Ogre data
    std::unordered_map<EntityID, CameraInfo> cameraMap; 
    std::unordered_map<EntityID, ViewportInfo> viewportMap; 
    std::unordered_map<EntityID, LightInfo> lightMap;

    std::vector<HUGO::OgreObjectID> freeOgreObjectId; // List of reusable IDs
    std::vector<std::size_t> idToIndex; // Mapping OgreObjectID to the PagedArray Index
    PagedArray<OgreObject, 128> ogreObjects; //Storage for renderable objects

    EntityID mainCameraEntity = 0;
    EntityID mainViewportEntity = 0;

    void updateViewportDimensions();

    ErrorOrVoid createOgreCamera(EntityID entity);
    ErrorOrVoid createOgreLight(EntityID entity);
    ErrorOrVoid createOgreViewport(EntityID entity);
    // void destroyOgreCamera(EntityID entity);
    // void destroyOgreViewport(EntityID entity);

    void handleTag();
    void updateAnimations(double dt);

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
    ~RenderSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(double) override;
    void update(double dt) override;

    Ogre::SceneManager* getSM() const;
END_HSYSTEM()

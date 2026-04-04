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
struct AnimationComponent;
struct ViewportComponent;
struct LightComponent;
struct CameraComponent;

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
        Ogre::Entity* ogreEntity = nullptr;
        Ogre::SceneNode* ogreSceneNode = nullptr;   // Scene node for transform
        std::unordered_map<HUGO::String, Ogre::AnimationState*> animationStates; //Stored animation states
        HUGO::String meshName;
    };

    //Ogre References
    Ogre::Root* ogreRoot;
    Ogre::SceneManager* sceneManager = nullptr;

    //Mapping ECS entities with their corresponding Ogre data
    std::unordered_map<EntityID, CameraInfo> cameraMap; 
    std::unordered_map<EntityID, ViewportInfo> viewportMap; 
    std::unordered_map<EntityID, LightInfo> lightMap;

    std::unordered_map<EntityID, OgreObject> ogreObjects;


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
    ErrorOrVoid createOgreObject(EntityID id, const std::string& meshName, const std::string& materialName="");
    void removeOgreObject(EntityID id);
    void destroyOgreObject(OgreObject& obj);
    void clearAnimationStates(OgreObject &obj);
                   
    // Global light configuration
    void setAmbientLight(float r, float g, float b);

    // Main camera and viewport configuration
    void setMainCamera(EntityID cameraEntity);
    
    void setMainViewport(EntityID viewportEntity);
      
    //Component Events
    void onAddRenderComponet(RenderComponent& rc);
    void onUpdateRenderComponet(RenderComponent& rc);
    void onRemoveRenderComponet(RenderComponent& rc);

    void onUpdateAnimationComponet(AnimationComponent& animComp);
    void onRemoveAnimationComponet(AnimationComponent& animComp);

    void onAddLightComponent(LightComponent& light);
    void onUpdateLightComponent(LightComponent& light);
    void onRemoveLightComponent(LightComponent& light);

    void onAddViewportComponent(ViewportComponent& viewport);
    void onUpdateViewportComponent(ViewportComponent& viewport);
    void onRemoveViewportComponent(ViewportComponent& viewport);

    void onAddCameraComponent(CameraComponent& camComp);
    void onRemoveCameraComponent(CameraComponent& camComp);

public:
    ~RenderSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(double) override;
    void update(double dt) override;

    Ogre::SceneManager* getSM() const;
END_HSYSTEM()

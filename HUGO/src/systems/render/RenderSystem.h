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

HSYSTEM(RenderSystem)
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

    std::unordered_map<EntityID, OgreObject> ogreObjects;

    void updateAnimations(double dt);

    //Register/Unregister render entities
    void registerRenderEntity(RenderComponent& comp);
    void unregisterRenderEntity(RenderComponent& comp);

    void clearAnimationStates(OgreObject &obj);

    //Component Events
    void onUpdateRenderComponet(RenderComponent& rc);
    void onUpdateAnimationComponet(AnimationComponent& animComp);
    void onRemoveAnimationComponet(AnimationComponent& animComp);

public:
    ~RenderSystem() override;

    void init() override;
    void onLoad() override;
    
    void fixedUpdate(double) override;
    void update(double dt) override;

    Ogre::SceneManager* getSM() const;
END_HSYSTEM()

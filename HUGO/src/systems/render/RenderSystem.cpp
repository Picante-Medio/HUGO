#include "RenderSystem.h"

#include <iostream>

#include "OgreOverlaySystem.h"
#include "SDL_video.h"

#include "InternalScene.h"
#include "LuaBinder.h"

#include "OgreUtils.h"
#include "OgreModule.h"

#include "RenderComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"

#pragma region Destructor
RenderSystem::~RenderSystem()
{
    // Destroy all renderable Ogre objects
    for(auto& obj : ogreObjects){
        destroyOgreObject(obj.second);
        clearAnimationStates(obj.second);
    }
    ogreObjects.clear();
    
    sceneManager = nullptr;
}
#pragma endregion

#pragma region System Init
void RenderSystem::init(){

    ogreRoot = Ogre::Root::getSingletonPtr();
    
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    //components events
    scene.onAddComponent<RenderComponent>().connect(this, &RenderSystem::registerRenderEntity);
    scene.onAddComponent<AnimationComponent>().connect(this, &RenderSystem::onUpdateAnimationComponet);

    scene.onUpdateComponent<RenderComponent>().connect(this, &RenderSystem::onUpdateRenderComponet);
    scene.onUpdateComponent<AnimationComponent>().connect(this, &RenderSystem::onUpdateAnimationComponet);

    scene.onRemoveComponent<RenderComponent>().connect(this, &RenderSystem::unregisterRenderEntity); 
    scene.onRemoveComponent<AnimationComponent>().connect(this, &RenderSystem::onRemoveAnimationComponet);
}
#pragma endregion

#pragma region SystemLoad
void RenderSystem::onLoad(){
    
}
#pragma endregion

#pragma region Updates
// ==================== Update Animations ====================
void RenderSystem::updateAnimations(double dt)
{
    for(auto& pack : scene.query<RenderComponent, AnimationComponent>()) {
        AnimationComponent& animComp = pack.getConst<AnimationComponent&>();
        RenderComponent& rc = pack.getConst<RenderComponent&>();

        if(ogreObjects.find(rc.entity) == ogreObjects.end()) continue;
        
        OgreObject& ogreObj = ogreObjects[rc.entity];
        Ogre::Real time = static_cast<Ogre::Real>(dt/1000.0f);//ms->s

        //updates all enable animations
        for(auto& a: animComp.anims){
            const HUGO::String& animName = a.first;
            const AnimationInfo& animInfo = a.second;

            if(!animInfo.enable) continue;

            auto it = ogreObj.animationStates.find(animName);
            if(it == ogreObj.animationStates.end()) continue;
            it->second->addTime(time);
        }
    }
}
// ==================== Fixed Update ====================
void RenderSystem::fixedUpdate(double){}

// ==================== Update ====================
void RenderSystem::update(double dt)
{ 
    updateAnimations(dt);

    //Update transform of entity with renderComponent
    for(auto& pack : scene.query<TransformComponent,RenderComponent>()) {
        TransformComponent& tr=pack.getConst<TransformComponent&>();
        OgreObject& obj = ogreObjects[tr.entity];
        if(obj.ogreSceneNode){
            //update transform
            obj.ogreSceneNode->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getWorldPosition()));
            obj.ogreSceneNode->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getWorldRotation()));
            obj.ogreSceneNode->setScale(HUGO::OgreUtils::vector3ToOgre(tr.getWorldScale()));
        } 
    }
}

#pragma endregion

#pragma region OgreObject
// ==================== Create OgreObject ====================
void RenderSystem::registerRenderEntity(RenderComponent& comp)
{
    EntityID id = comp.entity;

    std::string meshName= comp.getMeshName();
    std::string materialName = comp.getMaterialName();

    Ogre::Entity* ent = nullptr;
    Ogre::SceneNode* node = nullptr;

    try {
        //create ogre entity
        ent = sceneManager->createEntity(meshName);
        if (!materialName.empty()) {
            ent->setMaterialName(materialName);
        }

        //create scene node
        node = sceneManager->getRootSceneNode()->createChildSceneNode();
        node->attachObject(ent);
    }
    catch (const Ogre::Exception& e){
        StringError err;
        err.errorMessage = "[RenderSystem] Failed to create OgreObject: "+ std::string(e.getDescription());
        if (ent) sceneManager->destroyEntity(ent);
        if (node) sceneManager->destroySceneNode(node);
        
        scene.log(std::make_shared<StringError>(err));
        return;
    }
    
    //create OgreObject
    OgreObject obj = {ent, node};
    obj.meshName = meshName;

    //store object
    ogreObjects[id] = std::move(obj);
}
// ==================== Destroy OgreObject ====================
void RenderSystem::destroyOgreObject(OgreObject &obj)
{
    if(obj.ogreSceneNode){
        
        sceneManager->destroySceneNode(obj.ogreSceneNode);
        obj.ogreSceneNode = nullptr;
    }
    if(obj.ogreEntity){
        sceneManager->destroyEntity(obj.ogreEntity);
        obj.ogreEntity = nullptr;
    }
}
void RenderSystem::unregisterRenderEntity(RenderComponent& comp)
{
    EntityID id = comp.entity;
    destroyOgreObject(ogreObjects[id]);
}

void RenderSystem::clearAnimationStates(OgreObject &obj)
{
    obj.animationStates.clear();
}
#pragma endregion

#pragma region RenderComponentEvents

void RenderSystem::onUpdateRenderComponet(RenderComponent &rc)
{
    auto it = ogreObjects.find(rc.entity);
    if(it != ogreObjects.end()){
        OgreObject& obj = it->second;

        //check if change the mesh
        if(obj.meshName != rc.meshName){
            //destroy OgreEntity
            Ogre::SceneNode* node = obj.ogreSceneNode;
            node->detachAllObjects();
            sceneManager->destroyEntity(obj.ogreEntity);
            
            //create a new OgreEntity with new current mesh
            obj.ogreEntity = sceneManager->createEntity(rc.meshName);
            node->attachObject(obj.ogreEntity);
            
            //update mesh name
            obj.meshName = rc.meshName;

            //clear animations
            if(scene.hasComponent<AnimationComponent>(rc.entity))
            {
                auto& animComp = scene.getComponent<AnimationComponent>(rc.entity);
                animComp.anims.clear();
                clearAnimationStates(obj);
            }
        }

        obj.ogreEntity->setMaterialName(rc.materialName);
    }
}
#pragma endregion

#pragma region AnimarionComponentEvents
void RenderSystem::onUpdateAnimationComponet(AnimationComponent &animComp)
{
    EntityID entity = animComp.entity;
    if(!scene.hasComponent<RenderComponent>(entity)) return;
    
    RenderComponent& rc = scene.getComponent<RenderComponent>(entity);
    if(ogreObjects.find(rc.entity) == ogreObjects.end()) return;
    
    OgreObject& ogreObj = ogreObjects[rc.entity];
    Ogre::Entity* ogre_ent = ogreObj.ogreEntity;

    //remove animations that does exist in the Component
    for(auto it = ogreObj.animationStates.begin(); it != ogreObj.animationStates.end(); )
    {
        //If does not exist animation, remove it
        if(animComp.anims.find(it->first) == animComp.anims.end())
            it = ogreObj.animationStates.erase(it);
        else
            ++it;
    }

    //Update or create animations that are in the component
    for(auto& anim : animComp.anims){
        const HUGO::String& animName = anim.first;
        const AnimationInfo& animInfo = anim.second;

        Ogre::AnimationState* animState = nullptr;

        auto it = ogreObj.animationStates.find(animName);

        //if animation does not exist in the current AnimationStates
        if(it == ogreObj.animationStates.end())
        {
            if(!ogre_ent->hasAnimationState(animName))
            {
                scene.log(std::make_shared<StringError>("[RenderSystem] Animation "+ animName + " does not exist in entity " + std::to_string(rc.entity)));
                continue;
            }
            //get a new AnimationSate from ogre
            animState = ogre_ent->getAnimationState(animName);
            ogreObj.animationStates[animName] = animState;
        }
        else
        {
            //get the AnimationSate
            animState = it->second;
        }

        //Update animation status with AnimationComponent information
        if(animInfo.enable != animState->getEnabled()) 
            animState->setEnabled(animInfo.enable);

        if(animInfo.loop != animState->getLoop()) 
            animState->setLoop(animInfo.loop);
                    
        if(animInfo.weight != animState->getWeight()) 
            animState->setWeight(animInfo.weight);
    }

}

void RenderSystem::onRemoveAnimationComponet(AnimationComponent &animComp)
{
    auto it = ogreObjects.find(animComp.entity);
    if(it != ogreObjects.end())
        clearAnimationStates(it->second);
}
#pragma endregion
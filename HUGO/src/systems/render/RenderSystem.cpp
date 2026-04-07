#include "RenderSystem.h"

#include <iostream>

#include "OgreOverlaySystem.h"
#include "SDL_video.h"

#include "Scene.h"
#include "LuaBinder.h"

#include "OgreUtils.h"
#include "OgreModule.h"

#include "RenderComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "LightComponent.h"

#pragma region Destructor
RenderSystem::~RenderSystem()
{
    //destroy all cameras
    for(auto& c: cameraMap){
        unregisterCameraEntity(scene->getComponent<CameraComponent>(c.first));
    }
    cameraMap.clear();

    //destroy all lights
    for(auto& l: lightMap){
        unregisterLightEntity(scene->getComponent<LightComponent>(l.first));
    }
    lightMap.clear();

    // Destroy all renderable Ogre objects
    for(auto& obj : ogreObjects){
        destroyOgreObject(obj.second);
        clearAnimationStates(obj.second);
    }
    ogreObjects.clear();

    if(sceneManager){
        ogreRoot->destroySceneManager(sceneManager);
        sceneManager = nullptr;
    }
}
#pragma endregion

#pragma region System Init
void RenderSystem::init(){

    ogreRoot = Ogre::Root::getSingletonPtr();
    
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    sceneManager->addRenderQueueListener(Ogre::OverlaySystem::getSingletonPtr());

    //components events
    scene->onAddComponent<RenderComponent>().connect(this, &RenderSystem::onAddRenderComponet);
    scene->onAddComponent<LightComponent>().connect(this, &RenderSystem::registerLightEntity);
    scene->onAddComponent<CameraComponent>().connect(this, &RenderSystem::registerCameraEntity);
    scene->onAddComponent<AnimationComponent>().connect(this, &RenderSystem::onUpdateAnimationComponet);

    scene->onUpdateComponent<RenderComponent>().connect(this, &RenderSystem::onUpdateRenderComponet);
    scene->onUpdateComponent<AnimationComponent>().connect(this, &RenderSystem::onUpdateAnimationComponet);
    scene->onUpdateComponent<LightComponent>().connect(this, &RenderSystem::onUpdateLightComponent);

    scene->onRemoveComponent<RenderComponent>().connect(this, &RenderSystem::onRemoveRenderComponet); 
    scene->onRemoveComponent<AnimationComponent>().connect(this, &RenderSystem::onRemoveAnimationComponet);
    scene->onRemoveComponent<LightComponent>().connect(this, &RenderSystem::unregisterLightEntity);
    scene->onRemoveComponent<CameraComponent>().connect(this, &RenderSystem::unregisterCameraEntity); 
    
    // Basic light
    setAmbientLight(0.6f, 0.6f, 0.6f);
    sceneManager->setDisplaySceneNodes(true);

    // Camera and viewport creation for testing
    EntityID CamEntity = scene->createEntity();
    
    scene->addComponent<TransformComponent>(CamEntity, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0, 0, 50));
    });

    scene->addComponent<CameraComponent>(CamEntity);
}
#pragma endregion

#pragma region SystemLoad
void RenderSystem::onLoad(){
    LuaBinder::getInstance()->registerMethod<RenderSystem, void, float, float, float>(*this, &RenderSystem::setAmbientLight, "SetAmbientLight");
}
#pragma endregion

#pragma region Updates
// ==================== Update Animations ====================
void RenderSystem::updateAnimations(double dt)
{
    for(auto& pack : scene->query<RenderComponent, AnimationComponent>()) {
        AnimationComponent& animComp = pack.getDeprecated<AnimationComponent&>();
        RenderComponent& rc = pack.getDeprecated<RenderComponent&>();

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
    for(auto& pack : scene->query<TransformComponent,RenderComponent>()) {
        TransformComponent& tr=pack.getDeprecated<TransformComponent&>();
        OgreObject& obj = ogreObjects[tr.entity];
        if(obj.ogreSceneNode){
            //update transform
            obj.ogreSceneNode->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getWorldPosition()));
            obj.ogreSceneNode->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getWorldRotation()));
            obj.ogreSceneNode->setScale(HUGO::OgreUtils::vector3ToOgre(tr.getWorldScale()));
        } 
    }

    // Camera update
    for(auto& pack : scene->query<TransformComponent, CameraComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        CameraComponent& camera = pack.getDeprecated<CameraComponent&>();
        
        auto it = cameraMap.find(camera.entity);
        if (it != cameraMap.end()) {
           //update node transform
            it->second.node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getPosition()));
            it->second.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getRotation()));
            
            // update projection parameters
            if (camera.getProjectionType() == HUGO::ProjectionType::Perspective) {
                it->second.ogreCamera->setFOVy(Ogre::Degree(camera.getFOVY()));
            }
            it->second.ogreCamera->setNearClipDistance(camera.getNearClip());
            it->second.ogreCamera->setFarClipDistance(camera.getFarClip());
        }
    }

    // changes in window
    std::shared_ptr<HUGO::IInput> input = scene->getInput();

    if(input->windowHasChanged()){
        updateViewportDimensions();
        input->clearWindowFlag();
    }
}

#pragma endregion

#pragma region OgreObject
// ==================== Create OgreObject ====================
ErrorOrVoid RenderSystem::createOgreObject(EntityID id, const std::string &meshName, const std::string &materialName)
{
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
        
        return ErrorOrVoid(err);
    }
    
    //create OgreObject
    OgreObject obj = {ent, node};
    obj.meshName = meshName;

    //store object
    ogreObjects[id] = std::move(obj);
    
    return ErrorOrVoid();
}
// ==================== Remove OgreObject ====================
void RenderSystem::removeOgreObject(EntityID id)
{
    if (ogreObjects.find(id) != ogreObjects.end()){

        destroyOgreObject(ogreObjects[id]);

        ogreObjects.erase(id);
    }
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


void RenderSystem::clearAnimationStates(OgreObject &obj)
{
    obj.animationStates.clear();
}
#pragma endregion

#pragma region Registering
// ==================== Register Camera Entity ====================
void RenderSystem::registerCameraEntity(CameraComponent& comp)
{
    EntityID entity = comp.entity;
    // check if its already registered
    if (cameraMap.find(entity) != cameraMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreCamera(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}

// ==================== Register Light Entity ====================
void RenderSystem::registerLightEntity(LightComponent& comp)
{
    EntityID entity = comp.entity;
    if (lightMap.find(entity) != lightMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreLight(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}
// ==================== Unregister Camera Entity ====================
void RenderSystem::unregisterCameraEntity(CameraComponent& comp)
{
    EntityID entity = comp.entity;
    //remove camera and destroy Ogre Camera
    auto it = cameraMap.find(entity);
    if (it != cameraMap.end()) {
        if(it->second.node){
            it->second.node->detachAllObjects();
            sceneManager->destroySceneNode(it->second.node);
            it->second.node = nullptr;
        }
        if(it->second.ogreCamera){
            sceneManager->destroyCamera(it->second.ogreCamera);
            it->second.ogreCamera = nullptr;
        }
        cameraMap.erase(it);
    }
}

// ==================== Unregister Light Entity ====================
void RenderSystem::unregisterLightEntity(LightComponent& comp)
{
    EntityID entity = comp.entity;
    auto it = lightMap.find(entity);
    if (it != lightMap.end()) {
        if(it->second.node){
            it->second.node->detachAllObjects();
            sceneManager->destroySceneNode(it->second.node);
            it->second.node = nullptr;
        }
        if(it->second.ogreLight){
            sceneManager->destroyLight(it->second.ogreLight);
            it->second.ogreLight = nullptr;
        }
        lightMap.erase(it);
    }
}
#pragma endregion

#pragma region  OgreCamera Creation 
// ==================== Create Ogre Camera ====================
ErrorOrVoid RenderSystem::createOgreCamera(EntityID entity)
{
    CameraInfo res;
    // Get the transformComponent and CameraComponent of the camera to set the initial configuration
    const TransformComponent* transformComp = nullptr;
    const CameraComponent* cameraComp = nullptr;

    try{
        transformComp = &scene->getComponent<TransformComponent>(entity);
        cameraComp = &scene->getComponent<CameraComponent>(entity);
    }
    catch(...){
        return ErrorOrVoid("[RenderSystem] Camera entity " + std::to_string(entity) + " missing required components");
    }

    // Unique name for the camera
    std::string camName = "Camera_" + std::to_string(entity);
    std::string nodeName = "CameraNode_" + std::to_string(entity);
    
    res.ogreCamera = sceneManager->createCamera(camName);

    // Default parameters
    res.ogreCamera->setNearClipDistance(cameraComp->getNearClip());
    res.ogreCamera->setFarClipDistance(cameraComp->getFarClip());
    res.ogreCamera->setAutoAspectRatio(cameraComp->getAutoAspectRatio());

    if (cameraComp->getProjectionType() == HUGO::ProjectionType::Perspective) {
        res.ogreCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
        res.ogreCamera->setFOVy(Ogre::Degree(cameraComp->getFOVY()));
    } else {
        res.ogreCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        res.ogreCamera->setOrthoWindow(cameraComp->getOrthographicDimensions().first, cameraComp->getOrthographicDimensions().second);
    }

    // create node and attach camera
    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreCamera);
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp->getPosition()));
    res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp->getRotation()));
    
    cameraMap[entity] = res;

    return ErrorOrVoid();
}
// ==================== Create Ogre Light ====================
ErrorOrVoid RenderSystem::createOgreLight(EntityID entity)
{
    LightInfo res;
    
    // Get the transformComponent and LightComponentof the light to set the initial configuration
    const TransformComponent* transformComp = nullptr;
    const LightComponent* lightComp = nullptr;

    try{
        transformComp = &scene->getComponent<TransformComponent>(entity);
        lightComp = &scene->getComponent<LightComponent>(entity);
    }
    catch(...){
        return ErrorOrVoid("[RenderSystem] Light entity "+ std::to_string(entity) + "missing required components");
    }

    // Unique name for the light
    std::string lightName = "Light_" + std::to_string(entity);
    std::string nodeName = "LightNode_" + std::to_string(entity);
    
    res.ogreLight = sceneManager->createLight(lightName);

    // Get Ogre light type from LightComponent
    Ogre::Light::LightTypes ogreType;
    switch (lightComp->getLightType()) {
        case HUGO::LightType::LT_POINT:
            ogreType = Ogre::Light::LT_POINT;
            break;
        case HUGO::LightType::LT_DIRECTIONAL:
            ogreType = Ogre::Light::LT_DIRECTIONAL;
            break;
        case HUGO::LightType::LT_SPOTLIGHT:
            ogreType = Ogre::Light::LT_SPOTLIGHT;
            break;
        default:
            ogreType = Ogre::Light::LT_POINT;
            break;
    }

    res.ogreLight->setType(ogreType);
    res.ogreLight->setDiffuseColour(Ogre::ColourValue(lightComp->getDiffuse().x, lightComp->getDiffuse().y, lightComp->getDiffuse().z, lightComp->getDiffuse().w));
    res.ogreLight->setSpecularColour(Ogre::ColourValue(lightComp->getSpecular().x, lightComp->getSpecular().y, lightComp->getSpecular().z, lightComp->getSpecular().w));

    // parameters for spot light
    if (lightComp->getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
        res.ogreLight->setSpotlightRange(Ogre::Radian(lightComp->getSpotInner().valueRadians())
            , Ogre::Radian(lightComp->getSpotOuter().valueRadians()), lightComp->getSpotFallOff());

        if (lightComp->getSpotNearClip() > 0) {
            res.ogreLight->setSpotlightNearClipDistance(lightComp->getSpotNearClip());
        }
    }
    
    // parameters for directional light
    if (lightComp->getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
        res.ogreLight->setAttenuation( lightComp->getAttenuation().w, lightComp->getAttenuation().x,  
                                       lightComp->getAttenuation().y, lightComp->getAttenuation().z);
    }

    // shadows
    if(lightComp->getShadowFarDistance() > 0){
        res.ogreLight->setShadowFarDistance(lightComp->getShadowFarDistance());
    }

    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreLight);
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp->getPosition()));

    // for dirrectonal light
    if (lightComp->getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
        res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp->getRotation()));
    }

    lightMap[entity] = res;
    return ErrorOrVoid();
}

#pragma endregion

#pragma region  Screen Changes
void RenderSystem::updateViewportDimensions()
{
    int w,h;
    SDL_GetWindowSize(OgreModule::Instance().getSDLWindow(), &w, &h);
    if (w > 0 && h > 0)
    {
        Ogre::RenderWindow* renderWindow = OgreModule::Instance().getOgreRenderWindow();
        renderWindow->resize(w, h);
        renderWindow->windowMovedOrResized();
    }
}

#pragma endregion

void RenderSystem::setAmbientLight(float r, float g, float b)
{
    sceneManager->setAmbientLight(Ogre::ColourValue(r, g, b));
}

Ogre::SceneManager *RenderSystem::getSM() const
{
    return sceneManager;
}

#pragma region RenderComponentEvents
void RenderSystem::onAddRenderComponet(RenderComponent &rc)
{
    EntityID entity = rc.entity;
    if (ogreObjects.find(entity) == ogreObjects.end()) {
        //create a new ogreObject
        ErrorOrVoid e = createOgreObject(entity, rc.meshName,rc.materialName);
        if (e.isError()) return;
    } 
}

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
            if(scene->hasComponent<AnimationComponent>(rc.entity))
            {
                auto& animComp = scene->getComponent<AnimationComponent>(rc.entity);
                animComp.anims.clear();
                clearAnimationStates(obj);
            }
        }

        obj.ogreEntity->setMaterialName(rc.materialName);
    }
}

void RenderSystem::onRemoveRenderComponet(RenderComponent &rc)
{
    auto it = ogreObjects.find(rc.entity);
    if(it != ogreObjects.end()){
        OgreObject& obj = it->second;
        destroyOgreObject(obj);
        ogreObjects.erase(it);
    }        
}
#pragma endregion

#pragma region AnimarionComponentEvents
void RenderSystem::onUpdateAnimationComponet(AnimationComponent &animComp)
{
    EntityID entity = animComp.entity;
    if(!scene->hasComponent<RenderComponent>(entity)) return;
    
    RenderComponent& rc = scene->getComponent<RenderComponent>(entity);
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
                std::cerr << "[RenderSystem] Animation " << animName << " does not exist in entity " << rc.entity << "\n";
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

#pragma region LightComponentEvents
void RenderSystem::onUpdateLightComponent(LightComponent &light)
{
    EntityID entity = light.entity;
    if(!scene->hasComponent<TransformComponent>(entity)){
        std::cerr<<"[RenderSystem] Light "<<entity<<" has not trasnform\n";
        return;
    }
    TransformComponent& tr =scene->getComponent<TransformComponent>(entity);
    
    auto it = lightMap.find(entity);
    if (it != lightMap.end()) {
        // update position
        it->second.node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getPosition()));
        
        // for dirrectonal light
        if (light.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
            it->second.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getRotation()));
        }

        // update diffuse and specular
        it->second.ogreLight->setDiffuseColour(Ogre::ColourValue(light.getDiffuse().x, light.getDiffuse().y, light.getDiffuse().z, light.getDiffuse().w));
        it->second.ogreLight->setSpecularColour(Ogre::ColourValue(light.getSpecular().x, light.getSpecular().y, light.getSpecular().z, light.getSpecular().w));
        
        // update attenuation
        it->second.ogreLight->setAttenuation(light.getAttenuation().x ,light.getAttenuation().w, light.getAttenuation().y, light.getAttenuation().z);
        
        if (light.getLightType() == HUGO::LightType::LT_SPOTLIGHT) {
            it->second.ogreLight->setSpotlightRange(Ogre::Radian(light.getSpotInner().valueRadians())
                , Ogre::Radian(light.getSpotOuter().valueRadians()), light.getSpotFallOff());

            if (light.getSpotNearClip() > 0) {
                it->second.ogreLight->setSpotlightNearClipDistance(light.getSpotNearClip());
            }
        }
        
        // parameters for directional light
        if (light.getLightType() != HUGO::LightType::LT_DIRECTIONAL) {
            it->second.ogreLight->setAttenuation( light.getAttenuation().w, light.getAttenuation().x,  
                                        light.getAttenuation().y, light.getAttenuation().z);
        }

        // shadows
        if(light.getShadowFarDistance() > 0){
            it->second.ogreLight->setShadowFarDistance(light.getShadowFarDistance());
        }
    }
}
#pragma endregion

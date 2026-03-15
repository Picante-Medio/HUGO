#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "RenderSystem.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "AnimationComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "ViewportComponent.h"
#include "LightComponent.h"
#include "RenderTags.h"
#include "HugoTypes.h"
#include "HugoError.h"
#include "OgreUtils.h"

#include "OgreModule.h"
#include "LuaBinder.h"

RenderSystem::~RenderSystem()
{
    for(auto& v: viewportMap){
        if(v.second.ogreViewport){
            OgreModule::Instance().getOgreRenderWindow()->removeViewport(v.second.ogreViewport->getZOrder());
            v.second.ogreViewport = nullptr;
        }
    }
    viewportMap.clear();

    for(auto& c: cameraMap){
        if(c.second.node){
            c.second.node->detachAllObjects();
            sceneManager->destroySceneNode(c.second.node);
            c.second.node = nullptr;
        }
        if(c.second.ogreCamera){
            sceneManager->destroyCamera(c.second.ogreCamera);
            c.second.ogreCamera = nullptr;
        }
    }
    cameraMap.clear();

    for(auto& l: lightMap){
        if(l.second.node){
            l.second.node->detachAllObjects();
            sceneManager->destroySceneNode(l.second.node);
            l.second.node = nullptr;
        }
        if(l.second.ogreLight){
            sceneManager->destroyLight(l.second.ogreLight);
            l.second.ogreLight = nullptr;
        }
    }
    lightMap.clear();

    for(std::size_t i =0; i<ogreObjects.size(); ++i){
        destroyOgreObject(ogreObjects[i]);
    }
    ogreObjects.clear();
    idToIndex.clear();
    freeOgreObjectId.clear();

    if(sceneManager){
        ogreRoot->destroySceneManager(sceneManager);
        sceneManager = nullptr;
    }
}

#pragma region Tags
void RenderSystem::handleTag()
{
    // Camera update
    for(auto& pack : scene->query<CameraDirtyTag, TransformComponent, CameraComponent>()) {
        CameraDirtyTag& tag = pack.get<CameraDirtyTag&>();
        TransformComponent& tr = pack.get<TransformComponent&>();
        CameraComponent& camera = pack.get<CameraComponent&>();
        EntityID entity = tag.entity;
        
        auto it = cameraMap.find(entity);
        if (it != cameraMap.end()) {
            //update node transform
            it->second.node->setPosition(OgreUtils::vector3ToOgre(tr.position));
            it->second.node->setOrientation(OgreUtils::quaternionToOgre(tr.rotation));
            
            // update projection parameters
            if (camera.getProjectionType() == HUGO::ProjectionType::Perspective) {
                it->second.ogreCamera->setFOVy(Ogre::Degree(camera.getFOVY()));
            }
            it->second.ogreCamera->setNearClipDistance(camera.getNearClip());
            it->second.ogreCamera->setFarClipDistance(camera.getFarClip());
        }
        // eliminate the tag
        scene->removeComponent<CameraDirtyTag>(entity);
    }

    // Light update
    for(auto&pack : scene->query<LightDirtyTag, LightComponent, TransformComponent>()){
        LightDirtyTag& tag = pack.get<LightDirtyTag&>();
        LightComponent& light = pack.get<LightComponent&>();
        TransformComponent& tr = pack.get<TransformComponent&>();
        EntityID entity = tag.entity;

        auto it = lightMap.find(entity);
        if (it != lightMap.end()) {
            // update position
            it->second.node->setPosition(OgreUtils::vector3ToOgre(tr.position));
            
            // for dirrectonal light
            if (light.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
                it->second.node->setOrientation(OgreUtils::quaternionToOgre(tr.rotation));
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
        // eliminate the tag
        scene->removeComponent<LightDirtyTag>(entity);
    }

    // Viewport update
    for(auto& pack : scene->query<ViewportDirtyTag, ViewportComponent>()) {
        ViewportDirtyTag& tag = pack.get<ViewportDirtyTag&>();
        ViewportComponent& viewport = pack.get<ViewportComponent&>();
        EntityID entity = tag.entity;
        
        auto it = viewportMap.find(entity);
        if (it != viewportMap.end() && it->second.ogreViewport) {
            // update dimensions
            it->second.ogreViewport->setDimensions(viewport.getLeft(), viewport.getTop(), viewport.getWidth(), viewport.getHeight());
            
            // update color
            it->second.ogreViewport->setBackgroundColour(Ogre::ColourValue(
                viewport.getClearColor().x, viewport.getClearColor().y, 
                viewport.getClearColor().z, viewport.getClearColor().w
            ));

            // update the camera if it has changed
            if (viewport.getCameraEntity() != it->second.lastCameraEntity) {
                auto camIt = cameraMap.find(viewport.getCameraEntity());
                if (camIt != cameraMap.end()) {
                    it->second.ogreViewport->setCamera(camIt->second.ogreCamera);
                    it->second.lastCameraEntity = viewport.getCameraEntity();
                }
            }
        }
        // eliminate the tag
        scene->removeComponent<ViewportDirtyTag>(entity);
    }

    //Material Update
    for(auto& pack : scene->query<MaterialDirtyTag, RenderComponent>()) {
        MaterialDirtyTag& material = pack.get<MaterialDirtyTag&>();
        RenderComponent& rc = pack.get<RenderComponent&>();

        if (rc.objectID != HUGO::INVALID_OGRE_OBJECT_ID) {
            rc.materialName=material.materialName;
            OgreObject& obj = getOgreObject(rc.objectID);
            obj.ogreEntity->setMaterialName(rc.materialName);

            scene->removeComponent<MaterialDirtyTag>(rc.entity);
        }  
    }
}

#pragma endregion

#pragma region OgreObject
ErrorOrType<HUGO::OgreObjectID> RenderSystem::createOgreObject(const std::string &meshName, const std::string &materialName)
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
        
        return ErrorOrType<HUGO::OgreObjectID>(err);
    }
    
    HUGO::OgreObjectID id;
    //reuse id
    if(!freeOgreObjectId.empty()){ 
        id= freeOgreObjectId.back();
        freeOgreObjectId.pop_back();
    }
    else{
        //generate new ID
        id = static_cast<HUGO::OgreObjectID>(idToIndex.size());
        idToIndex.push_back(0);
    }
    
    //create OgreObject
    OgreObject obj = {id, ent, node};

    //store object
    std::size_t index = ogreObjects.push_back(std::move(obj));
    idToIndex[id] = index;
    
    return ErrorOrType<HUGO::OgreObjectID>(id);
}

RenderSystem::OgreObject& 
RenderSystem::getOgreObject(HUGO::OgreObjectID id)
{
    return ogreObjects[idToIndex[id]];
}

void RenderSystem::removeOgreObject(HUGO::OgreObjectID& id)
{
    if (id != HUGO::INVALID_OGRE_OBJECT_ID){
        
        std::size_t index = idToIndex[id];
        std::size_t lastIndex = ogreObjects.size() - 1;

        destroyOgreObject(ogreObjects[index]);

        if (index != lastIndex) {
            ogreObjects[index] = std::move(ogreObjects[lastIndex]);
            HUGO::OgreObjectID movedID = ogreObjects[index].id;
            idToIndex[movedID] = index;
        }

        ogreObjects.remove(lastIndex);
        freeOgreObjectId.push_back(id);
        id = HUGO::INVALID_OGRE_OBJECT_ID;
    }
}

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
#pragma endregion

#pragma region Updates
void RenderSystem::updateAnimations(double dt)
{
    for(auto& pack : scene->query<RenderComponent, AnimationComponent>()) {
        AnimationComponent& animComp = pack.get<AnimationComponent&>();
        RenderComponent& rc = pack.get<RenderComponent&>();

        if(rc.objectID == HUGO::INVALID_OGRE_OBJECT_ID) continue;
        
        Ogre::Entity* ogre_ent = getOgreObject(rc.objectID).ogreEntity;

        for(auto& a: animComp.anims){
            const HUGO::String& animName = a.first;
            AnimationInfo animInfo = a.second;

            if(!ogre_ent->hasAnimationState(animName)) 
                std::cerr<<"[RenderSystem] Animation "+ animName + " does not exist in entity " + std::to_string(rc.entity) + "\n";
            
            Ogre::AnimationState* animState = ogre_ent->getAnimationState(animName);
            
            if(animInfo.enable != animState->getEnabled()) 
                animState->setEnabled(animInfo.enable);
                
            if(animInfo.loop!= animState->getLoop()) 
                animState->setLoop(animInfo.loop);

            if(animInfo.enable)
                animState->addTime(static_cast<Ogre::Real>(dt));
        }
    }
}

void RenderSystem::init(){

        ogreRoot = Ogre::Root::getSingletonPtr();
    try {
        sceneManager = ogreRoot->createSceneManager();
    }
    catch (const Ogre::Exception& e) {
        throw std::runtime_error("[RenderSystem] SceneManager creation failed: " + std::string(e.getDescription()));
    }
    ogreRoot->_setCurrentSceneManager(sceneManager);
    sceneManager->addRenderQueueListener(Ogre::OverlaySystem::getSingletonPtr());

    // Basic light
    setAmbientLight(0.2f, 0.2f, 0.2f);


    // Camera and viewport creation for testing
    EntityID defaultCamera = createCameraEntity(HUGO::Vector3(0, 0, 50));
    setMainCamera(defaultCamera);

    EntityID defaultViewport = createViewportEntity();    
    setMainViewport(defaultViewport);

    createLightEntity( HUGO::Vector3(0, 20, 20), //light position
                        HUGO::Vector4(0.9f, 0.9f, 0.8f, 0.2f), //diffuse
                        HUGO::Vector4(0.0f, 0.0f, 0.0f, 0.0f), //specular
                        HUGO::LightType::LT_POINT, 
                        HUGO::Radian(0), 
                        HUGO::Radian(0), 
                        0, 
                        0.1f, 
                        0.1f, 
                        100); 
        

}

void RenderSystem::onLoad(){
    LuaBinder& luaBinder = LuaBinder::Instance();
    luaBinder.registerMethod<RenderSystem, void, float, float, float>(*this, &RenderSystem::setAmbientLight, "SetAmbientLight");
}

void RenderSystem::fixedUpdate(double dt)
{

}

void RenderSystem::update(double dt)
{ 
    handleTag();

    //Update transform of entity with renderComponent
    for(auto& pack : scene->query<TransformComponent,RenderComponent>()) {
        RenderComponent& rc = pack.get<RenderComponent&>();
        TransformComponent& tr=pack.get<TransformComponent&>();

        if (rc.objectID == HUGO::INVALID_OGRE_OBJECT_ID) {
            //create a new ogreObject
            ErrorOrType<HUGO::OgreObjectID> e = createOgreObject(rc.meshName,rc.materialName);
            if (e.isError()) {
                rc.objectID=HUGO::INVALID_OGRE_OBJECT_ID;
                std::cerr<< e.getError().errorMessage;
                continue;
            }
            rc.objectID = e.getValue();
        } 
        OgreObject& obj = getOgreObject(rc.objectID);

        //update transform
        obj.ogreSceneNode->setPosition(OgreUtils::vector3ToOgre(tr.position));
        obj.ogreSceneNode->setOrientation(OgreUtils::quaternionToOgre(tr.rotation));
        obj.ogreSceneNode->setScale(OgreUtils::vector3ToOgre(tr.scale));
    }

    updateAnimations(dt);
}

#pragma endregion

// Helper function for Camera creation
EntityID RenderSystem::createCameraEntity(const HUGO::Vector3& position)
{
    EntityID entity = scene->createEntity();
    
    TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(entity);
    transform.position = position;
    scene->addComponent<CameraComponent>(entity);
    
    registerCameraEntity(entity);
    
    return entity;
}

// Helper function for Viewport creation
EntityID RenderSystem::createViewportEntity()
{
    EntityID entity = scene->createEntity();
    
    scene->addComponent<ViewportComponent>(entity);
    
    registerViewportEntity(entity);
    
    return entity;
}

EntityID RenderSystem::createLightEntity(const HUGO::Vector3 &position, HUGO::Vector4 diff, HUGO::Vector4 specular, HUGO::LightType lType, const HUGO::Radian &outer, const HUGO::Radian &inner, float falloff, float nearClip, float shadowNearClipDist, float shadowFarClipDist)
{
    EntityID entity = scene->createEntity();
    TransformComponent& tr = scene->addAndGetComponent<TransformComponent>(entity);
    tr.position=position;

    LightComponent& light = scene->addAndGetComponent<LightComponent>(entity);
    light.setDiffuse(diff);
    light.setSpecular(specular);
    light.setLightType(lType);
    light.setSpotOuter(outer);
    light.setSpotInner(inner);
    light.setSpotFallOff(falloff);
    light.setSpotNearClip(nearClip);
    light.setShadowNearClipDist(shadowNearClipDist);
    light.setShadowFarClipDist(shadowFarClipDist);

    registerLightEntity(entity);
    
    return entity;
}

// ==================== Registering ====================
#pragma region Registering
void RenderSystem::registerCameraEntity(EntityID entity)
{
    // check if its already registered
    if (cameraMap.find(entity) != cameraMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreCamera(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}

void RenderSystem::registerViewportEntity(EntityID entity)
{
    //check if its already registered
    if (viewportMap.find(entity) != viewportMap.end()) {
        return;
    }
    
    createOgreViewport(entity);
}

void RenderSystem::registerLightEntity(EntityID entity)
{
    if (lightMap.find(entity) != lightMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreLight(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}

void RenderSystem::unregisterCameraEntity(EntityID entity)
{
    //destroyOgreCamera(entity);
    cameraMap.erase(entity);
    
    if (mainCameraEntity == entity) {
        mainCameraEntity = 0;
    }
}

void RenderSystem::unregisterViewportEntity(EntityID entity)
{
    //destroyOgreViewport(entity);
    viewportMap.erase(entity);
    
    if (mainViewportEntity == entity) {
        mainViewportEntity = 0;
    }
}
#pragma endregion

// ==================== Ogre Camera and ViewPort Creation ====================
#pragma region  OgreCamera and ViewPort Creation 
ErrorOrVoid RenderSystem::createOgreCamera(EntityID entity)
{
    CameraInfo res;
    
    // Get the transformComponent and CameraComponent of the camera to set the initial configuration
    const TransformComponent* transformComp = nullptr;
    const CameraComponent* cameraComp = nullptr;
    for(auto& pack : scene->query<TransformComponent, CameraComponent>()) {
        const TransformComponent& tr = pack.get<TransformComponent&>();
        const CameraComponent& cam = pack.get<CameraComponent&>();
        if (tr.entity == entity && cam.entity == entity) {
            transformComp = &tr;
            cameraComp = &cam;
            break;
        }
    }

    if (!transformComp || !cameraComp) {
        // gestion de errores
        return ErrorOrVoid("[RenderSystem] Camera entity "+ std::to_string(entity) + "missing required components");
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
    res.node->setPosition(OgreUtils::vector3ToOgre(transformComp->position));
    res.node->setOrientation(OgreUtils::quaternionToOgre(transformComp->rotation));
    
    cameraMap[entity] = res;
    
    // Añadir tag para sincronización futura
    scene->addComponent<CameraDirtyTag>(entity);
    return ErrorOrVoid();
}

ErrorOrVoid RenderSystem::createOgreLight(EntityID entity)
{
    LightInfo res;
    
    // Get the transformComponent and LightComponentof the light to set the initial configuration
    const TransformComponent* transformComp = nullptr;
    const LightComponent* lightComp = nullptr;
    for(auto& pack : scene->query<TransformComponent, LightComponent>()) {
        const TransformComponent& tr = pack.get<TransformComponent&>();
        const LightComponent& light = pack.get<LightComponent&>();
        if (tr.entity == entity && light.entity == entity) {
            transformComp = &tr;
            lightComp = &light;
            break;
        }
    }

    if (!transformComp || !lightComp) {
        // gestion de errores
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
    res.node->setPosition(OgreUtils::vector3ToOgre(transformComp->position));

    // for dirrectonal light
    if (lightComp->getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
        res.node->setOrientation(OgreUtils::quaternionToOgre(transformComp->rotation));
    }

    lightMap[entity] = res;
    return ErrorOrVoid();
}

void RenderSystem::createOgreViewport(EntityID entity)
{
    ViewportInfo res;
    
    const ViewportComponent* viewportComp = nullptr;
    for(auto& pack : scene->query<ViewportComponent>()) {
        const ViewportComponent& vp = pack.get<ViewportComponent&>();
        if (vp.entity == entity) {
            viewportComp = &vp;
            break;
        }
    }
    
    Ogre::Camera* cam = nullptr;
    
    if (viewportComp->getCameraEntity() != 0) {
        auto camIt = cameraMap.find(viewportComp->getCameraEntity());
        if (camIt != cameraMap.end()) {
            cam = camIt->second.ogreCamera;
            res.lastCameraEntity = viewportComp->getCameraEntity();
        }
    }
    
    // if it has no camera asigned, use the main one
    if (!cam && mainCameraEntity != 0) {
        
        auto camIt = cameraMap.find(mainCameraEntity);
        if (camIt != cameraMap.end()) {
            cam = camIt->second.ogreCamera;
        }
    }

    // if the viewport doesnt have a camera, asign the firts avaible
    if (!cam && !cameraMap.empty()) {
        cam = cameraMap.begin()->second.ogreCamera;
    }

    // create ogre viewport
    res.ogreViewport = OgreModule::Instance().getOgreRenderWindow()->addViewport(
        cam,
        viewportComp->getZOrder(),
        viewportComp->getLeft(),
        viewportComp->getTop(),
        viewportComp->getWidth(),
        viewportComp->getHeight()
    );

    // background color
    res.ogreViewport->setBackgroundColour(Ogre::ColourValue(
        viewportComp->getClearColor().x,
        viewportComp->getClearColor().y,
        viewportComp->getClearColor().z,
        viewportComp->getClearColor().w
    ));
    
    cam->setAspectRatio(float(res.ogreViewport->getActualWidth()) / float(res.ogreViewport->getActualHeight()));
    
    viewportMap[entity] = res;

    scene->addComponent<ViewportDirtyTag>(entity);
}
#pragma endregion

#pragma region  Screen Changes
void RenderSystem::updateViewportDimensions()
{
    for (auto& [entity, res] : viewportMap) {
        if (res.ogreViewport) {
            res.ogreViewport->_updateDimensions();

            // upsate cameras asociated to the viewport
            Ogre::Camera* cam = res.ogreViewport->getCamera();
            if (cam) {
                cam->setAspectRatio(float(res.ogreViewport->getActualWidth()) / 
                                    float(res.ogreViewport->getActualHeight()));
            }
        }
    }
}

#pragma endregion

void RenderSystem::setAmbientLight(float r, float g, float b)
{
    sceneManager->setAmbientLight(Ogre::ColourValue(r, g, b));
}

void RenderSystem::setMainCamera(EntityID cameraEntity)
{
    if (cameraMap.find(cameraEntity) != cameraMap.end()) {
        mainCameraEntity = cameraEntity;
    
        for (auto& [entity, res] : viewportMap) {
            if (res.ogreViewport) {
                res.ogreViewport->setCamera(cameraMap[cameraEntity].ogreCamera);
            }
        }
    }
}

void RenderSystem::setMainViewport(EntityID viewportEntity)
{
    if (viewportMap.find(viewportEntity) != viewportMap.end()) {
        mainViewportEntity = viewportEntity;
    }
}

Ogre::SceneManager *RenderSystem::getSM() const
{
    return sceneManager;
}

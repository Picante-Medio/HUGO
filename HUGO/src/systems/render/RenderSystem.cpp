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
#include "ViewportComponent.h"
#include "LightComponent.h"

#include "RenderTags.h"

#pragma region Destructor
RenderSystem::~RenderSystem()
{
    //detroy all viewports
    for(auto& v: viewportMap){
        if(v.second.ogreViewport){
            OgreModule::Instance().getOgreRenderWindow()->removeViewport(v.second.ogreViewport->getZOrder());
            v.second.ogreViewport = nullptr;
        }
    }
    viewportMap.clear();

    //destroy all cameras
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

    //destroy all lights
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

    // Destroy all renderable Ogre objects
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
#pragma endregion

#pragma region System Init
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
    setAmbientLight(0.6f, 0.6f, 0.6f);
    sceneManager->setDisplaySceneNodes(true);

    // Camera and viewport creation for testing
    EntityID CamEntity = scene->createEntity();
    
    TransformComponent& transform = scene->addAndGetComponent<TransformComponent>(CamEntity);
    transform.position = HUGO::Vector3(0, 0, 50);
    scene->addComponent<CameraComponent>(CamEntity);
    
    registerCameraEntity(CamEntity);
    setMainCamera(CamEntity);

    EntityID ViewEntity = scene->createEntity();
    
    scene->addComponent<ViewportComponent>(ViewEntity);
    
    registerViewportEntity(ViewEntity);  
    setMainViewport(ViewEntity);
}
#pragma endregion

#pragma region SystemLoad
void RenderSystem::onLoad(){
    LuaBinder::getInstance()->registerMethod<RenderSystem, void, float, float, float>(*this, &RenderSystem::setAmbientLight, "SetAmbientLight");
}
#pragma endregion

#pragma region Tags
void RenderSystem::handleTag()
{
    for(auto& pack : scene->query<LightCreateDirtyTag, TransformComponent, LightComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        EntityID entity = tr.entity;
        auto it = lightMap.find(entity);
        if (it == lightMap.end()) {
           registerLightEntity(entity);
        }
        // eliminate the tag
        scene->removeComponent<LightCreateDirtyTag>(entity);
    }

     for(auto& pack : scene->query<ViewportCreateDirtyTag, ViewportComponent>()) {
        ViewportComponent& view = pack.getDeprecated<ViewportComponent&>();
        EntityID entity = view.entity;
        auto it = viewportMap.find(entity);
        if (it == viewportMap.end()) {
           registerViewportEntity(entity);
        }
        // eliminate the tag
        scene->removeComponent<ViewportCreateDirtyTag>(entity);
    }
    // Light update
    for(auto&pack : scene->query<LightDirtyTag, LightComponent, TransformComponent>()){
        LightDirtyTag& tag = pack.getDeprecated<LightDirtyTag&>();
        LightComponent& light = pack.getDeprecated<LightComponent&>();
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        EntityID entity = tag.entity;

        auto it = lightMap.find(entity);
        if (it != lightMap.end()) {
            // update position
            it->second.node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.position));
            
            // for dirrectonal light
            if (light.getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
                it->second.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.rotation));
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
        ViewportDirtyTag& tag = pack.getDeprecated<ViewportDirtyTag&>();
        ViewportComponent& viewport = pack.getDeprecated<ViewportComponent&>();
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
        MaterialDirtyTag& material = pack.getDeprecated<MaterialDirtyTag&>();
        RenderComponent& rc = pack.getDeprecated<RenderComponent&>();

        if (rc.objectID != HUGO::INVALID_OGRE_OBJECT_ID) {
            OgreObject& obj = getOgreObject(rc.objectID);
            obj.ogreEntity->setMaterialName(rc.materialName);

            scene->removeComponent<MaterialDirtyTag>(rc.entity);
        }  
    }

    //Animation Update
    for(auto& pack : scene->query<AnimationComponent,AnimationDirtyTag,RenderComponent>())
    {
        AnimationComponent& animComp = pack.getDeprecated<AnimationComponent&>();
        RenderComponent& rc = pack.getDeprecated<RenderComponent&>();

        if(rc.objectID == HUGO::INVALID_OGRE_OBJECT_ID) continue;
        
        OgreObject& ogreObj = getOgreObject(rc.objectID);
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

        scene->removeComponent<AnimationDirtyTag>(animComp.entity);
    }

}

#pragma endregion

#pragma region Updates
// ==================== Update Animations ====================
void RenderSystem::updateAnimations(double dt)
{
    for(auto& pack : scene->query<RenderComponent, AnimationComponent>()) {
        AnimationComponent& animComp = pack.getDeprecated<AnimationComponent&>();
        RenderComponent& rc = pack.getDeprecated<RenderComponent&>();

        if(rc.objectID == HUGO::INVALID_OGRE_OBJECT_ID) continue;
        
        OgreObject& ogreObj = getOgreObject(rc.objectID);
        Ogre::Real time = static_cast<Ogre::Real>(dt);

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
    handleTag();
    updateAnimations(dt);

    //Update transform of entity with renderComponent
    for(auto& pack : scene->query<TransformComponent,RenderComponent>()) {
        RenderComponent& rc = pack.getDeprecated<RenderComponent&>();
        TransformComponent& tr=pack.getDeprecated<TransformComponent&>();

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
        obj.ogreSceneNode->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getWorldPosition()));
        obj.ogreSceneNode->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getWorldRotation()));
        obj.ogreSceneNode->setScale(HUGO::OgreUtils::vector3ToOgre(tr.getWorldScale()));
    }

    // Camera update
    for(auto& pack : scene->query<TransformComponent, CameraComponent>()) {
        TransformComponent& tr = pack.getDeprecated<TransformComponent&>();
        CameraComponent& camera = pack.getDeprecated<CameraComponent&>();
        
        auto it = cameraMap.find(camera.entity);
        if (it == cameraMap.end()) {
            registerCameraEntity(camera.entity);
        }
        else{
            //update node transform
            it->second.node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.position));
            it->second.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.rotation));
            
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
// ==================== Get OgreObject ====================
RenderSystem::OgreObject& RenderSystem::getOgreObject(HUGO::OgreObjectID id)
{
    return ogreObjects[idToIndex[id]];
}
// ==================== Remove OgreObject ====================
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
#pragma endregion

#pragma region Registering
// ==================== Register Camera Entity ====================
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
// ==================== Register ViewPort Entity ====================
void RenderSystem::registerViewportEntity(EntityID entity)
{
    //check if its already registered
    if (viewportMap.find(entity) != viewportMap.end()) {
        return;
    }
    
    ErrorOrVoid e = createOgreViewport(entity);
    if(e.isError()){
        std::cerr<<e.getError().errorMessage;
    }
}
// ==================== Register Light Entity ====================
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
// ==================== Unregister Camera Entity ====================
void RenderSystem::unregisterCameraEntity(EntityID entity)
{
    //destroyOgreCamera(entity);
    cameraMap.erase(entity);
    
    if (mainCameraEntity == entity) {
        mainCameraEntity = 0;
    }
}
// ==================== Unregister ViewPort Entity ====================
void RenderSystem::unregisterViewportEntity(EntityID entity)
{
    //destroyOgreViewport(entity);
    viewportMap.erase(entity);
    
    if (mainViewportEntity == entity) {
        mainViewportEntity = 0;
    }
}
#pragma endregion

#pragma region  OgreCamera and ViewPort Creation 
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
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp->position));
    res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp->rotation));
    
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
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp->position));

    // for dirrectonal light
    if (lightComp->getLightType() == HUGO::LightType::LT_DIRECTIONAL) {
        res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp->rotation));
    }

    lightMap[entity] = res;
    return ErrorOrVoid();
}
// ==================== Create Ogre Viewport ====================
ErrorOrVoid RenderSystem::createOgreViewport(EntityID entity)
{
    ViewportInfo res;    
    const ViewportComponent* viewportComp = nullptr;
    try{
        viewportComp = &scene->getComponent<ViewportComponent>(entity);
    }
    catch(...){
        return ErrorOrVoid("[RenderSystem] Viewport entity "+ std::to_string(entity) + "missing required components");
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
    
    res.ogreViewport->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    
    cam->setAspectRatio(float(res.ogreViewport->getActualWidth()) / float(res.ogreViewport->getActualHeight()));
    
    viewportMap[entity] = res;

    scene->addComponent<ViewportDirtyTag>(entity);
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

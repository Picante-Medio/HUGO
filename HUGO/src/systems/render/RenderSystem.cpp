#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#include "RenderSystem.h"
#include "PhysicsTags.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "ViewportComponent.h"
#include "LightComponent.h"
#include "RenderTags.h"
#include "HugoTypes.h"
#include "HugoError.h"
#include "OgreUtils.h"

RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> ogreRoot, Ogre::RenderWindow* window)
    :System(scene), ogreRoot(ogreRoot), renderWindow(window), idToIndex(), freeOgreObjectId()
{
    try {
        sceneManager = ogreRoot->createSceneManager();
    }
    catch (const Ogre::Exception& e) {
        throw std::runtime_error("[RenderSystem] SceneManager creation failed: " + std::string(e.getDescription()));
    }
    
    //sceneManager->setDisplaySceneNodes(true);

    // Basic light
    setAmbientLight(0.15f, 0.15f, 0.15f);
    const Ogre::ConfigOptionMap& options = ogreRoot->getRenderSystem()->getConfigOptions();

    auto it = options.find("Video Mode");

    if (it != options.end()) {
        for (const auto& value : it->second.possibleValues) {

            int width = 0;
            int height = 0;

            char temp = 0;

            std::stringstream sStream(value);
            sStream >> width >> temp >> height;

            if (width > 0 && height > 0)
                resolutions.emplace_back(width, height);
        }
    }

    // Camera and viewport creation for testing
    EntityID defaultCamera = createCameraEntity(HUGO::Vector3(0, 0, 200));
    setMainCamera(defaultCamera);

    EntityID defaultViewport = createViewportEntity();    
    setMainViewport(defaultViewport);

    createLightEntity( HUGO::Vector3(-50, 300, 800), HUGO::Vector4(0.4f, 0.2f, 0.4f, 0.2f)
        , HUGO::Vector4(0.0f, 0.0f, 0.0f, 0.0f), HUGO::LightType::LT_POINT, HUGO::Radian(0), HUGO::Radian(0), 0, 0.1f, 0.1f, 300);   

    // createTestEntity();
}

RenderSystem::~RenderSystem()
{
    for(auto& v: viewportMap){
        if(v.second.ogreViewport){
            renderWindow->removeViewport(v.second.ogreViewport->getZOrder());
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
HUGO::OgreObjectID RenderSystem::createOgreObject(const std::string &meshName, const std::string &materialName)
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
        std::cerr << "[RenderSystem] Failed to create OgreObject: "<< std::string(e.getDescription()) << '\n';
        if (ent) sceneManager->destroyEntity(ent);
        if (node) sceneManager->destroySceneNode(node);

        return HUGO::INVALID_OGRE_OBJECT_ID;
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

    return id;
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
            rc.objectID = createOgreObject(rc.meshName,rc.materialName);

            if (rc.objectID == HUGO::INVALID_OGRE_OBJECT_ID) {
                std::cerr << "[RenderSystem] RenderComponent could not create OgreObject"<<'\n';
                continue;
            }
        } 
        OgreObject& obj = getOgreObject(rc.objectID);

        //update transform
        obj.ogreSceneNode->setPosition(OgreUtils::vector3ToOgre(tr.position));
        obj.ogreSceneNode->setOrientation(OgreUtils::quaternionToOgre(tr.rotation));
        obj.ogreSceneNode->setScale(OgreUtils::vector3ToOgre(tr.scale));
    }

    for(auto& pack : scene->query<TransformComponent,RenderComponent,DynamicRBTag>()) {
        RenderComponent& rc = pack.get<RenderComponent&>();

        std::cout << "Render position: " << getOgreObject(rc.objectID).ogreSceneNode->getPosition().x << " " << getOgreObject(rc.objectID).ogreSceneNode->getPosition().y << " " << getOgreObject(rc.objectID).ogreSceneNode->getPosition().z << "\n";
    }
}

#pragma endregion

#pragma region Testing
void RenderSystem::createTestEntity()
{
    EntityID e1 = scene->createEntity();
    scene->addComponent<TransformComponent>(e1, HUGO::Vector3(-30,10,10), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.2f,0.2f,0.2f));
    scene->addComponent<RenderComponent>(e1, "cube.mesh");

    EntityID e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2, HUGO::Vector3(30,10,10), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.2f,0.2f,0.2f));
    scene->addComponent<RenderComponent>(e2, "sphere.mesh","prueba/ColorRosa");

    setMaterial(e1, "prueba/ColorVerde");
}
#pragma endregion

void RenderSystem::setMaterial(EntityID entity, const std::string &materialName)
{
    scene->addComponent<MaterialDirtyTag>(entity, materialName);
}

// Helper function for Camera creation
EntityID RenderSystem::createCameraEntity(const HUGO::Vector3& position)
{
    EntityID entity = scene->createEntity();
    
    scene->addComponent<TransformComponent>(entity,position);
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
    scene->addComponent<TransformComponent>(entity,position);
    scene->addComponent<LightComponent>(entity, diff, specular, lType, outer, inner, falloff, nearClip, shadowNearClipDist, shadowFarClipDist);
    
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
    
    createOgreCamera(entity);
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
    
    createOgreLight(entity);
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
void RenderSystem::createOgreCamera(EntityID entity)
{
    CameraInfo res;
    
    // Get the transform component of the camera to set the initial position
    const TransformComponent* transformComp = nullptr;
    for(auto& pack : scene->query<TransformComponent>()) {
        const TransformComponent& tr = pack.get<TransformComponent&>();
        if (tr.entity == entity) {
            transformComp = &tr;
            break;
        }
    }

    // Get CameraComponent
    const CameraComponent* cameraComp = nullptr;
    for(auto& pack : scene->query<CameraComponent>()) {
        const CameraComponent& cam = pack.get<CameraComponent&>();
        if (cam.entity == entity) {
            cameraComp = &cam;
            break;
        }
    }

    if (!transformComp || !cameraComp) {
        // gestion de errores
        std::cerr<<"[RenderSystem] Camera entity "<< std::to_string(entity) << "missing required components"<<"\n";
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
}

void RenderSystem::createOgreLight(EntityID entity)
{
    LightInfo res;
    
    // Get the transform component of the light to set the initial position
    const TransformComponent* transformComp = nullptr;
    for(auto& pack : scene->query<TransformComponent>()) {
        const TransformComponent& tr = pack.get<TransformComponent&>();
        if (tr.entity == entity) {
            transformComp = &tr;
            break;
        }
    }

    // Get LightComponent
    const LightComponent* lightComp = nullptr;
    for(auto& pack : scene->query<LightComponent>()) {
        const LightComponent& light = pack.get<LightComponent&>();
        if (light.entity == entity) {
            lightComp = &light;
            break;
        }
    }

    if (!transformComp || !lightComp) {
        // gestion de errores
        std::cerr<<"[RenderSystem] Light entity "<< std::to_string(entity) << "missing required components"<<"\n";
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
    res.ogreViewport = renderWindow->addViewport(
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


void RenderSystem::setfullScreen()
{
     if (!renderWindow) return;
    auto [w, h] = resolutions[resolutions.size() - 1];
    renderWindow->setFullscreen(true, w, h);
    
    // Update all viewports
    updateViewportDimensions();

}

void RenderSystem::setWindowed(bool fullScreen)
{
    if (!renderWindow) return;
    Ogre::RenderWindowDescription desc = ogreRoot->getRenderSystem()->getRenderWindowDescription();
    renderWindow->setFullscreen(fullScreen, desc.width, desc.height);

    // Update all viewports
    updateViewportDimensions();
}

void RenderSystem::resizeScreen(unsigned int widthPt, unsigned int heightPt)
{
    if (!renderWindow) return;
    renderWindow->resize(widthPt, heightPt);

    // Update all viewports
    updateViewportDimensions();
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

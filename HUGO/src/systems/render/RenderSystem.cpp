#include <OgreCommon.h>
#include <OgreConfigOptionMap.h>
#include <iostream>

#include "RenderSystem.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "ViewportComponent.h"
#include "CameraTags.h"
#include "HugoTypes.h"

RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> ogreRoot, Ogre::RenderWindow* window)
    :System(scene), ogreRoot(ogreRoot), renderWindow(window), idToIndex(), freeOgreObjectId()
{
    try {
        sceneManager = ogreRoot->createSceneManager();
    }
    catch (const Ogre::Exception& e) {
        throw std::runtime_error("[RenderSystem] SceneManager creation failed: " + std::string(e.getDescription()));
    }
    
    //para ver los ejes
    //sceneManager->setDisplaySceneNodes(true);

    // Basic light
    sceneManager->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    Ogre::Light* light = sceneManager->createLight();
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDiffuseColour(0.7f, 0.7f, 0.7f);
    Ogre::SceneNode* nodeDir = sceneManager->getRootSceneNode()->createChildSceneNode();
    nodeDir->setDirection(Ogre::Vector3(-1, -1, 0));
    nodeDir->attachObject(light);
    

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
}

RenderSystem::~RenderSystem()
{
   
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
            it->second.node->setPosition(tr.position.toOgre());
            it->second.node->setOrientation(tr.rotation.toOgre());
            
            // update projection parameters
            if (camera.projectionType == HUGO::ProjectionType::Perspective) {
                it->second.ogreCamera->setFOVy(Ogre::Degree(camera.fovY));
            }
            it->second.ogreCamera->setNearClipDistance(camera.nearClip);
            it->second.ogreCamera->setFarClipDistance(camera.farClip);
        }
        // eliminate the tag
        scene->removeComponent<CameraDirtyTag>(entity);
    }

    // Viewport update
    for(auto& pack : scene->query<ViewportDirtyTag, ViewportComponent>()) {
        ViewportDirtyTag& tag = pack.get<ViewportDirtyTag&>();
        ViewportComponent& viewport = pack.get<ViewportComponent&>();
        EntityID entity = tag.entity;
        
        auto it = viewportMap.find(entity);
        if (it != viewportMap.end() && it->second.ogreViewport) {
            // update dimensions
            it->second.ogreViewport->setDimensions(viewport.left, viewport.top,viewport.width, viewport.height);
            
            // update color
            it->second.ogreViewport->setBackgroundColour(Ogre::ColourValue(
                viewport.clearColor.x, viewport.clearColor.y, 
                viewport.clearColor.z, viewport.clearColor.w
            ));

            // update the camera if it has changed
            if (viewport.cameraEntity != it->second.lastCameraEntity) {
                auto camIt = cameraMap.find(viewport.cameraEntity);
                if (camIt != cameraMap.end()) {
                    it->second.ogreViewport->setCamera(camIt->second.ogreCamera);
                    it->second.lastCameraEntity = viewport.cameraEntity;
                }
            }
        }
        // eliminate the tag
        //scene->removeComponent<ViewportDirtyTag>(entity);
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

OgreObject &RenderSystem::getOgreObject(HUGO::OgreObjectID id)
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
        obj.ogreSceneNode->setPosition(tr.position.toOgre());
        obj.ogreSceneNode->setOrientation(tr.rotation.toOgre());
        obj.ogreSceneNode->setScale(tr.scale.toOgre());
    }

}

void RenderSystem::fixedUpdate(double dt)
{
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
// EntityID RenderSystem::createCameraEntity()
// {
//     EntityID entity = scene->createEntity();
    
//     scene->addComponent<TransformComponent>(entity);
//     scene->addComponent<CameraComponent>(entity);
    
//     registerCameraEntity(entity);
    
//     return entity;
// }
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
    CameraResources res;
    
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
    res.ogreCamera->setNearClipDistance(cameraComp->nearClip);
    res.ogreCamera->setFarClipDistance(cameraComp->farClip);
    res.ogreCamera->setAutoAspectRatio(cameraComp->autoAspectRatio);
    
    if (cameraComp->projectionType == HUGO::ProjectionType::Perspective) {
        res.ogreCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
        res.ogreCamera->setFOVy(Ogre::Degree(cameraComp->fovY));
    } else {
        res.ogreCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        res.ogreCamera->setOrthoWindow(cameraComp->orthoWidth, cameraComp->orthoHeight);
    }

    // create node and attach camera
    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreCamera);
    res.node->setPosition(transformComp->position.toOgre());
    res.node->setOrientation(transformComp->rotation.toOgre());
    
    cameraMap[entity] = res;
    
    // Añadir tag para sincronización futura
    scene->addComponent<CameraDirtyTag>(entity);
}

void RenderSystem::createOgreViewport(EntityID entity)
{
    ViewportResources res;
    
    const ViewportComponent* viewportComp = nullptr;
    for(auto& pack : scene->query<ViewportComponent>()) {
        const ViewportComponent& vp = pack.get<ViewportComponent&>();
        if (vp.entity == entity) {
            viewportComp = &vp;
            break;
        }
    }
    
    Ogre::Camera* cam = nullptr;
    
    if (viewportComp->cameraEntity != 0) {
        auto camIt = cameraMap.find(viewportComp->cameraEntity);
        if (camIt != cameraMap.end()) {
            cam = camIt->second.ogreCamera;
            res.lastCameraEntity = viewportComp->cameraEntity;
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
        viewportComp->zOrder,
        viewportComp->left,
        viewportComp->top,
        viewportComp->width,
        viewportComp->height
    );

    // background color
    res.ogreViewport->setBackgroundColour(Ogre::ColourValue(
        viewportComp->clearColor.x,
        viewportComp->clearColor.y,
        viewportComp->clearColor.z,
        viewportComp->clearColor.w
    ));
    
    cam->setAspectRatio(float(res.ogreViewport->getActualWidth()) / float(res.ogreViewport->getActualHeight()));
    
    viewportMap[entity] = res;

    scene->addComponent<ViewportDirtyTag>(entity);
}
#pragma endregion

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
    

    // Cosa rara para poner fullscreen en Windows
    // SDL_DisplayMode dm;
    // SDL_GetDesktopDisplayMode(0, &dm);
    
    // renderWindow->reposition(0, 0);
    // renderWindow->resize(dm.w, dm.h);
    
    // #ifdef _WIN32
    //     HWND hwnd = nullptr;
    //     renderWindow->getCustomAttribute("WINDOW", &hwnd);
    //     if (hwnd) {
    //         SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    //         SetWindowPos(hwnd, HWND_TOP, 0, 0, dm.w, dm.h, SWP_FRAMECHANGED);
    //     }
    // #endif
    
    // renderWindow->windowMovedOrResized();
    
    // if (viewport) {
    //     viewport->_updateDimensions();
    //     Ogre::Camera* cam = viewport->getCamera();
    //     if (cam) {
    //         cam->setAspectRatio(float(dm.w) / float(dm.h));
    //     }
    // }

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

EntityID RenderSystem::getMainCamera() const
{
    return mainCameraEntity;
}

void RenderSystem::setMainViewport(EntityID viewportEntity)
{
    if (viewportMap.find(viewportEntity) != viewportMap.end()) {
        mainViewportEntity = viewportEntity;
    }
}

EntityID RenderSystem::getMainViewport() const
{
    return mainViewportEntity;
}

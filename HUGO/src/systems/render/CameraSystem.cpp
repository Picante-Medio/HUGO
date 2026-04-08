#include "CameraSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "OgreUtils.h"
#include "OgreModule.h"

void CameraSystem::init(){

    ogreRoot = Ogre::Root::getSingletonPtr();
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    scene->onAddComponent<CameraComponent>().connect(this, &CameraSystem::registerCameraEntity);
    scene->onRemoveComponent<CameraComponent>().connect(this, &CameraSystem::unregisterCameraEntity); 

    //From the moment
    EntityID CamEntity = scene->createEntity();
    
    scene->addComponent<TransformComponent>(CamEntity, [](TransformComponent& transform){
        transform.setPosition(HUGO::Vector3(0, 0, 50));
    });
    scene->addComponent<CameraComponent>(CamEntity);
}

void CameraSystem::update(double dt){
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
}

CameraSystem::~CameraSystem(){
    //destroy all cameras
    for(auto& c: cameraMap){
        unregisterCameraEntity(scene->getComponent<CameraComponent>(c.first));
    }
    cameraMap.clear();
}

void CameraSystem::registerCameraEntity(CameraComponent& cameraComp){
    EntityID entity = cameraComp.entity;
    CameraInfo res;
    // Get the transformComponent and CameraComponent of the camera to set the initial configuration

    if(!scene->hasComponent<TransformComponent>(entity)){
        scene->log(std::make_shared<StringError>("[RenderSystem] Camera entity " + std::to_string(entity) + " missing required components"));
        return;
    }
    TransformComponent& transformComp = scene->getComponent<TransformComponent>(entity);

    // Unique name for the camera
    std::string camName = "Camera_" + std::to_string(entity);
    std::string nodeName = "CameraNode_" + std::to_string(entity);
    
    res.ogreCamera = sceneManager->createCamera(camName);

    // Default parameters
    res.ogreCamera->setNearClipDistance(cameraComp.getNearClip());
    res.ogreCamera->setFarClipDistance(cameraComp.getFarClip());
    res.ogreCamera->setAutoAspectRatio(cameraComp.getAutoAspectRatio());

    if (cameraComp.getProjectionType() == HUGO::ProjectionType::Perspective) {
        res.ogreCamera->setProjectionType(Ogre::PT_PERSPECTIVE);
        res.ogreCamera->setFOVy(Ogre::Degree(cameraComp.getFOVY()));
    } else {
        res.ogreCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        res.ogreCamera->setOrthoWindow(cameraComp.getOrthographicDimensions().first, cameraComp.getOrthographicDimensions().second);
    }

    // create node and attach camera
    res.node = sceneManager->getRootSceneNode()->createChildSceneNode(nodeName);
    res.node->attachObject(res.ogreCamera);
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp.getPosition()));
    res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp.getRotation()));

    // create ogre viewport
    res.ogreViewport = OgreModule::Instance().getOgreRenderWindow()->addViewport(
        res.ogreCamera,
        cameraComp.getZOrder(),
        cameraComp.getLeft(),
        cameraComp.getTop(),
        cameraComp.getWidth(),
        cameraComp.getHeight()
    );
    
    // background color
    res.ogreViewport->setBackgroundColour(Ogre::ColourValue(
        cameraComp.getClearColor().x,
        cameraComp.getClearColor().y,
        cameraComp.getClearColor().z,
        cameraComp.getClearColor().w
    ));
    
    res.ogreViewport->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    
    cameraComp.setAspectRatio(float(res.ogreViewport->getActualWidth()) / float(res.ogreViewport->getActualHeight()));
    
    cameraMap[entity] = res;
}

void CameraSystem::unregisterCameraEntity(CameraComponent& comp){
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
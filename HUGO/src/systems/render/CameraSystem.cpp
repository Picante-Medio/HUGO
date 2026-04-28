#include "CameraSystem.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "OgreUtils.h"
#include "OgreModule.h"

void CameraSystem::init(){

    ogreRoot = Ogre::Root::getSingletonPtr();
    sceneManager = OgreModule::Instance().getCurrentSceneManager();

    scene.onAddComponent<CameraComponent>().connect(this, &CameraSystem::registerCameraEntity);
    scene.onRemoveComponent<CameraComponent>().connect(this, &CameraSystem::unregisterCameraEntity); 
}

void CameraSystem::update(uint32_t){
    //If there are no cameras, throw error
    if(cameraMap.size() == 0){
        scene.log(std::make_shared<StringError>("[CameraSystem] No cameras in scene"));
    }
    if(m_mainCamera == NULL){
        scene.log(std::make_shared<StringError>("[CameraSystem] Main camera is not set"));
    }

    // Camera update
    for(auto& pack : scene.query<TransformComponent, CameraComponent>()) {
        TransformComponent& tr = pack.getConst<TransformComponent&>();
        CameraComponent& camera = pack.getConst<CameraComponent&>();

        cameraMap[camera.entity].node->setPosition(HUGO::OgreUtils::vector3ToOgre(tr.getWorldPosition()));
        cameraMap[camera.entity].node->setOrientation(HUGO::OgreUtils::quaternionToOgre(tr.getWorldRotation()));

        // update projection parameters
        if (camera.getProjectionType() == HUGO::ProjectionType::Perspective) {
            cameraMap[camera.entity].ogreCamera->setFOVy(Ogre::Degree(camera.getFOVY()));
        }
        cameraMap[camera.entity].ogreCamera->setNearClipDistance(camera.getNearClip());
        cameraMap[camera.entity].ogreCamera->setFarClipDistance(camera.getFarClip());

        //Check if it is a different mainCamera tham the actual one. If it is, then change it
        if(camera.isMain() && camera.entity != m_mainCamera) {
            pack.modify<CameraComponent>([this](CameraComponent& camera) {
                setMainCamera(camera);
            });
        } 
    }
}

CameraSystem::~CameraSystem(){
    //destroy all cameras
    for(auto& c: cameraMap){
        destroyCamera(c.first,c.second);
    }
    cameraMap.clear();
    
    sceneManager = nullptr;
}

void CameraSystem::registerCameraEntity(CameraComponent& cameraComp){
    EntityID entity = cameraComp.entity;
    CameraInfo res;
    // Get the transformComponent and CameraComponent of the camera to set the initial configuration

    if(!scene.hasComponent<TransformComponent>(entity)){
        scene.log(std::make_shared<StringError>("[CameraSystem] Camera entity " + std::to_string(entity) + " missing required components"));
        return;
    }
    TransformComponent& transformComp = scene.getComponent<TransformComponent>(entity);

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
    res.node->setPosition(HUGO::OgreUtils::vector3ToOgre(transformComp.getWorldPosition()));
    res.node->setOrientation(HUGO::OgreUtils::quaternionToOgre(transformComp.getWorldRotation()));

    cameraMap[entity] = res;


    if(cameraComp.isMain()){
        setMainCamera(cameraComp);
    }
}
void CameraSystem::destroyCamera(EntityID entity, CameraInfo& camInfo)
{
    if(camInfo.node){
        camInfo.node->detachAllObjects();
        sceneManager->destroySceneNode(camInfo.node);
        camInfo.node = nullptr;
    }

    if(camInfo.ogreCamera){
        sceneManager->destroyCamera(camInfo.ogreCamera);
        camInfo.ogreCamera = nullptr;
    }

    //If it was main cam, change it for the default blank camera. It is the user´s fault to not change the mainCamera before deleting the actual one
    if(entity == m_mainCamera){
        OgreModule::Instance().viewportToDefaultCamera();
    }
}

void CameraSystem::unregisterCameraEntity(CameraComponent& comp){
    EntityID entity = comp.entity;
    //remove camera and destroy Ogre Camera
    destroyCamera(entity, cameraMap[entity]);
    cameraMap.erase(entity);
}

void CameraSystem::setMainCamera(CameraComponent& camComp){
    EntityID camID = camComp.entity;
    Ogre::Viewport* viewport = OgreModule::Instance().getViewport();
    viewport->setCamera(cameraMap[camID].ogreCamera);

    viewport->setBackgroundColour(Ogre::ColourValue(
        camComp.getClearColor().x,
        camComp.getClearColor().y,
        camComp.getClearColor().z,
        camComp.getClearColor().w
    ));

    m_mainCamera = camID;
}
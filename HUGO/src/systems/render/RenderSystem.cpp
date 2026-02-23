#include <iostream>

#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"
#include "OgreLight.h"

#include "RenderSystem.h"
#include "Scene.h"
#include "RenderComponent.h"
#include "TransformComponent.h"



RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> ogreRoot, Ogre::RenderWindow* window)
    :System(scene), ogreRoot(ogreRoot), renderWindow(window)
{
    sceneManager = ogreRoot.get()->createSceneManager();
    
    //para ver las ejes
    sceneManager->setDisplaySceneNodes(true);

    // Camera
    Ogre::Camera* mCam = sceneManager->createCamera("camera");

    mCam->setNearClipDistance(1);
    mCam->setNearClipDistance(1);
    mCam->setFarClipDistance(10000);
    mCam->setAutoAspectRatio(true);

    defaultCameraNode = sceneManager->getRootSceneNode()->createChildSceneNode("cameraNode");
    defaultCameraNode->attachObject(mCam);
    defaultCameraNode->setPosition(0, 0, 200);

    // Viewport
    viewport = renderWindow->addViewport(mCam);
    viewport->setBackgroundColour(Ogre::ColourValue(0.3f, 0.3f, 0.5f));
    viewport->setCamera(mCam);

    mCam->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));   

    // Basic light
    sceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

    Ogre::Light* light = sceneManager->createLight();
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDiffuseColour(1.0, 1.0, 1.0);
    Ogre::SceneNode* nodeDir = sceneManager->getRootSceneNode()->createChildSceneNode();
    nodeDir->setDirection(Ogre::Vector3(-1, -1, -1));
    nodeDir->attachObject(light);
    

    const auto& options = ogreRoot->getRenderSystem()->getConfigOptions();

    auto it = options.find("Video Mode");

    if (it != options.end()) {
        for (const auto& value : it->second.possibleValues) {

            int width = 0;
            int height = 0;

            sscanf(value.c_str(), "%d x %d", &width, &height);

            if (width > 0 && height > 0)
                resolutions.emplace_back(width, height);
        }
    }
   
}

void RenderSystem::handleTag()
{
    for(auto pack : scene->query<TransformComponent,RenderComponent>()) {
        RenderComponent& rc = pack.get<RenderComponent&>();
        EntityID entity = rc.entity;

        //if do not have create ogre entity
        if(!rc.ogreEntity){
            OgreObject obj = createOgreObject(entity,rc.meshName,rc.materialName);
            rc.ogreEntity = obj.ogreEntity;
            rc.ogreSceneNode=obj.sceneNode;
        }

        //update render transform
        TransformComponent& tr=pack.get<TransformComponent&>();
        rc.ogreSceneNode->setPosition(tr.position.toOgre());
        rc.ogreSceneNode->setOrientation(tr.rotation.toOgre());
        rc.ogreSceneNode->setScale(tr.scale.toOgre());
    }
}

OgreObject RenderSystem::createOgreObject(EntityID entity, const std::string &meshName, const std::string &materialName)
{
    //create ogre entity
    Ogre::Entity* ent=sceneManager->createEntity(meshName);  
    if(!materialName.empty()) 
        ent->setMaterialName(materialName);

    //create scene node
    Ogre::SceneNode* node= sceneManager->getRootSceneNode()->createChildSceneNode();
    node->attachObject(ent);

    return {ent, node};
}

void RenderSystem::update(double dt)
{
   handleTag();

    ////test
    float vel = 0.01;
    for(auto pack : scene->query<TransformComponent,RenderComponent>()) {
        TransformComponent& tr=pack.get<TransformComponent&>();
        tr.position.x += vel*dt;
    }
    ////
}

void RenderSystem::fixedUpdate(double dt)
{
}

void RenderSystem::createTestEntity()
{
    EntityID e = scene->createEntity();
    scene->addComponent<TransformComponent>(e, HUGO::Vector3(-30,10,10), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.2,0.2,0.2));
    scene->addComponent<RenderComponent>(e, "cube.mesh");


    EntityID e2 = scene->createEntity();
    scene->addComponent<TransformComponent>(e2, HUGO::Vector3(30,10,10), HUGO::Quaternion(1,0,0,0), HUGO::Vector3(0.2,0.2,0.2));
    scene->addComponent<RenderComponent>(e2, "sphere.mesh");
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
    
    viewport->_updateDimensions();

}

void RenderSystem::setWindowed(bool fullScreen)
{
    if (!renderWindow) return;
    auto desc = ogreRoot->getRenderSystem()->getRenderWindowDescription();
    renderWindow->setFullscreen(fullScreen, desc.width, desc.height);
}

void RenderSystem::resizeScreen(unsigned int widthPt, unsigned int heightPt)
{
    renderWindow->resize(widthPt, heightPt);

    viewport->_updateDimensions();
}

void RenderSystem::setBackgroundColor(float r, float g, float b)
{
    viewport->setBackgroundColour(Ogre::ColourValue(r, g, b));
}

void RenderSystem::setCameraPosition(float x, float y, float z)
{
    defaultCameraNode->setPosition(x, y, z);
}

void RenderSystem::setAmbientLight(float r, float g, float b)
{
    sceneManager->setAmbientLight(Ogre::ColourValue(r, g, b));
}

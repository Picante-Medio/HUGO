#include "Ogre.h"

#include <iostream>

#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"

#include "RenderSystem.h"
#include "Scene.h"
#include "OgreObjectComponent.h"
#include "CreateOgreObjectComponent.h"



RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> ogreRoot, Ogre::RenderWindow* window)
    :System(scene),ogreRoot(ogreRoot), renderWindow(window)
{
    sceneManager = ogreRoot.get()->createSceneManager();

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

    mCam->setAspectRatio(float(viewport->getActualWidth()) / float(viewport->getActualHeight()));   

    viewport->setCamera(mCam);

    // Basic light
    sceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));


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
    // std::vector<EntityID> toRemove;

    // Query pack = scene.get()->query<CreateOgreObjectComponent>();
    // for (auto& c : pack) {
       
    //     CreateOgreObjectComponent& msg = c.get<CreateOgreObjectComponent>();
        
    //     // //create ogre entity with menssage data
    //     // std::unique_ptr<Ogre::Entity> ent= std::unique_ptr<Ogre::Entity>(sceneManager->createEntity(msg.meshName));
    //     // if(!msg.meshName.empty()) 
    //     //     ent->setMaterialName(msg.materialName);
       
    //     // //create scene node
    //     // std::unique_ptr<Ogre::SceneNode> node= std::unique_ptr<Ogre::SceneNode>(sceneManager->getRootSceneNode()->createChildSceneNode());
    //     // node->attachObject(ent.get());

    //     // //add component with ogre data to entity
    //     // scene.get()->addComponent<OgreObjectComponent>(msg.entity, std::move(ent), std::move(node));
        
    //     // //store need remove menssageComponent entityId
    //     // toRemove.push_back(msg.entity);
    // }

    // //remove messageComponent
    // for(EntityID e: toRemove){
    //     //scene.get()->removeComponent<CreateOgreObjectComponent>(e);
    // }

}

void RenderSystem::update(double dt)
{
   handleTag();
}

void RenderSystem::fixedUpdate(double dt)
{
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

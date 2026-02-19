#include "Ogre.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"

#include "RenderSystem.h"
#include "Scene.h"

#include "../components/render/OgreObjectComponent.h"
#include "../components/messages/CreateOgreObjectComponent.h"

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

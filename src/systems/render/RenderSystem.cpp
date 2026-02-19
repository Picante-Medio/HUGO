#include "Ogre.h"
#include "OgreSceneManager.h"
#include "OgreRenderWindow.h"

#include "RenderSystem.h"
#include "Scene.h"

#include "../components/render/OgreObjectComponent.h"
#include "../components/menssage/CreateOgreObjectComponent.h"

RenderSystem::RenderSystem(std::shared_ptr<Scene> scene, std::shared_ptr<Ogre::Root> ogreRoot)
    :System(scene),ogreRoot(ogreRoot)
{
        sceneManager = ogreRoot.get()->createSceneManager();
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

#include "Component.h"
#include <memory>

namespace Ogre {
    class Entity;
    class SceneNode;
}

struct OgreObjectComponent: public Component
{
    std::unique_ptr<Ogre::Entity> ogreEntity;
    std::unique_ptr<Ogre::SceneNode> ogreSceneNode;

    OgreObjectComponent(EntityID entity, 
                        std::unique_ptr<Ogre::Entity> ogreEntity,
                        std::unique_ptr<Ogre::SceneNode> node) 
        : Component(entity), ogreEntity(std::move(ogreEntity)), ogreSceneNode(std::move(node)){};
};
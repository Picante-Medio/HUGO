#include "Component.h"
#include <memory>
#include <string>

namespace Ogre {
    class Entity;
    class SceneNode;
}

struct RenderComponent: public Component
{
    std::string meshName;
    std::string materialName;

    Ogre::Entity* ogreEntity = nullptr;
    Ogre::SceneNode* ogreSceneNode = nullptr;

    RenderComponent(EntityID entity, 
                        const std::string& meshName,
                        const std::string& materialName="") 
        : Component(entity), meshName(meshName), materialName(materialName){};
};
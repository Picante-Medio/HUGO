#pragma once
#include "HugoTypes.h"
#include <memory>
#include "miniaudio.h"
#include "OgreRoot.h"

namespace HUGO{
    //When changing to a Scene, ResourceManager must be called to unload previously allocated memory and load assets required for the new Scene
    class ResourceManager{
        
        std::shared_ptr<ma_engine> m_miniAudio_engine;
        std::shared_ptr<Ogre::Root> m_ogreRoot;

        public:
        ResourceManager(std::shared_ptr<ma_engine> ma_eng, std::shared_ptr<Ogre::Root> ogreRoot);
        void unloadSceneResources(HUGO::SceneID id);
        void loadSceneResources(HUGO::SceneID id);

        //Setup resources from all scenes and prefabs of game
        void setupResources(HUGO::String path);
    };
}
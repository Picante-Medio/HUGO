#include "ResourceManager.h"
#include "OgreRoot.h"

using namespace HUGO;

ResourceManager::ResourceManager(std::shared_ptr<ma_engine> ma_eng, std::shared_ptr<Ogre::Root> ogreRoot): m_miniAudio_engine(ma_eng), m_ogreRoot(ogreRoot){
    
}

void ResourceManager::unloadSceneResources(HUGO::SceneID id){

}

void ResourceManager::loadSceneResources(HUGO::SceneID id){

}
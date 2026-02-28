#include "ResourceManager.h"

using namespace HUGO;

ResourceManager::ResourceManager(std::shared_ptr<ma_engine> ma_eng, std::shared_ptr<Ogre::Root> ogreRoot): m_miniAudio_engine(ma_eng), m_ogreRoot(ogreRoot){
    
}

void ResourceManager::unloadSceneResources(HUGO::SceneID id){
    //Unload all the resources from Miniaudio and Ogre
}

void ResourceManager::loadSceneResources(HUGO::SceneID id){
    //It should search in the map for the assets that are used, such as meshes, textures, audio... and call Ogre and MiniAudio to load them in memory.

}

void ResourceManager::setupResources(HUGO::String path){
    
}
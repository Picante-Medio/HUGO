#include "ResourceManager.h"
#include "OgreRoot.h"
#include "OgreResourceGroupManager.h"

using namespace HUGO;

ResourceManager::ResourceManager(std::shared_ptr<ma_engine> ma_eng, std::shared_ptr<Ogre::Root> ogreRoot): m_miniAudio_engine(ma_eng), m_ogreRoot(ogreRoot), m_chatParser(){
    
}

void ResourceManager::unloadSceneResources(HUGO::String sceneName){
    //Unload all the resources from Miniaudio and Ogre

}

void ResourceManager::loadSceneResources(HUGO::String sceneName){
    //It should search in the map for the assets that are used, such as meshes, textures, audio... and call Ogre and MiniAudio to load them in memory.
    
}

void ResourceManager::setupResourcesInPath(HUGO::String path){
    m_chatParser.readChatFolder(path);
    const std::unordered_map<HUGO::String, HUGO::SceneData>& sceneInfo = m_chatParser.getAllSceneData();
    
    for(auto scene : sceneInfo){
        const HUGO::String& sceneName = scene.first;
        const HUGO::SceneData& sceneData = scene.second;


        //resource folder must already be in resources.cfg
        for(auto resource : sceneData.resources){
            
        }

    }
}
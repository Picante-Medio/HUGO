#include "ChatParser.h"
#include "ResourceManager.h"
#include "Scene.h"
#include "Ecs.h"
#include <any>
#include "Audio3DInfoComponet.h"
#include "AudioComponent.h"
#include "RenderComponent.h"
#include "ViewportComponent.h"

#include <iostream>
#include <filesystem>

using namespace HUGO;


ChatParser::ChatParser(): m_scene_data(), m_prefab_data(){

}

void ChatParser::readChatFolder(const HUGO::String& chatFolder){

    for(const auto& entry : std::filesystem::directory_iterator(chatFolder)) {
        HUGO::String chatPath = chatFolder + static_cast<HUGO::String>(entry.path().filename());

        readChat(chatPath);
    }
}

void ChatParser::readChat(const HUGO::String& chatPath){
    //To do
}

EntityData ChatParser::getPrefabData(const HUGO::String& prefabName){
    std::unordered_map<HUGO::String, EntityData>::iterator res = m_prefab_data.find(prefabName);

    if(res == m_prefab_data.end()) {
        //No existe lo que se pide
    }

    return res->second;

}

SceneData ChatParser::getSceneData(const HUGO::String & sceneName){
    std::unordered_map<HUGO::String, SceneData>::iterator res = m_scene_data.find(sceneName);
    
    if(res == m_scene_data.end()) {
        //No existe lo que se pide
    }

    return res->second;
}
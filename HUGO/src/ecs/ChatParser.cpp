#include "ChatParser.h"
#include <any>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

using namespace HUGO;


ChatParser::ChatParser(): m_scene_data(), m_prefab_data(){

}

void ChatParser::readChatFolder(const HUGO::String& chatFolder){

    for(const auto& entry : std::filesystem::directory_iterator(chatFolder)) {
        //! Doesn't compile: no suitable user-defined conversion from "std::filesystem::path" to "HUGO::String" (aka "std::string") exists
        HUGO::String chatPath = chatFolder + static_cast<HUGO::String>(entry.path().filename().generic_string());

        readChat(chatPath);
    }
}

void ChatParser::readChat(const HUGO::String& chatPath){
    
    /*
    std::ifstream file(chatPath);
    if(!file.is_open()){
        //Error opening file
    }

    std::streambuf* cinbuf = std::cin.rdbuf(file.rdbuf());

    //while not end of file
    HUGO::String sceneName = "\0";

    HUGO::String input;
    
    std::cin >> input;
    while(std::cin){

        if(input == "SCENE") std::cin >> sceneName;

        if(input == "COMPONENT") m_scene_data[sceneName].

        std::cin >> input;
    }

    std::cin.rdbuf(cinbuf);
    */
}

void ChatParser::handleType(const HUGO::String& type){

    
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

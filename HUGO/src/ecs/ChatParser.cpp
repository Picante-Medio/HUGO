#include "ChatParser.h"
#include <any>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

using namespace HUGO;


ChatParser::ChatParser(): m_sceneData(), m_prefabData(){

}

void ChatParser::readChatFolder(const HUGO::String& chatFolder){

    for(const auto& entry : std::filesystem::directory_iterator(chatFolder)) {
        //! Doesn't compile: no suitable user-defined conversion from "std::filesystem::path" to "HUGO::String" (aka "std::string") exists
        HUGO::String chatPath = chatFolder + static_cast<HUGO::String>(entry.path().filename().generic_string());

        readChat(chatPath);
    }
}

void ChatParser::readChat(const HUGO::String& chatPath){
    
    
    std::ifstream file(chatPath);
    if(!file.is_open()){
        //Error opening file
    }

    std::streambuf* cinbuf = std::cin.rdbuf(file.rdbuf());

    
    HUGO::String chatType;

    std::cin >> chatType;

    if(chatType == "SCENE"){
        handleSceneType();
    }
    else if(chatType == "PREFAB"){
        handlePrefabType();
    }

    std::cin.rdbuf(cinbuf);
    file.close();
}

void ChatParser::handleSceneType(){
    HUGO::String sceneName;
    std::cin >> sceneName;

    //Initialize scene data of name sceneName
    m_sceneData[sceneName].entities = std::vector<EntityData>();
    m_sceneData[sceneName].systems = std::vector<HUGO::String>();
    m_sceneData[sceneName].resources = std::vector<HUGO::String>();
    
    HUGO::String input;
    std::cin >> input;

    while(std::cin){

        if(input == "RESOURCE"){
            HUGO::String resourcePath;
            std::cin >> resourcePath;

            m_sceneData[sceneName].resources.push_back(resourcePath);
        }

        if (input == "ENTITY"){
            std::vector<HUGO::EntityData>& sceneEntities = m_sceneData[sceneName].entities;
            sceneEntities.push_back(EntityData());
        }

        if(input == "COMPONENT"){
            ComponentData compData;
            std::cin >> compData.componentName;
            m_sceneData[sceneName].entities.back().push_back(compData);
        }

        if(input == "VECTOR_3"){
            Vector3 vec3 = readVector3();

            //Might be terrorism
            //Adds it to the parameters of the last component added to the last entity of the scene
            m_sceneData[sceneName].entities.back().back().parameters.push_back(vec3);
        }

        if(input == "FLOAT"){
            HUGO::Float f = readFloat();

            m_sceneData[sceneName].entities.back().back().parameters.push_back(f);
        }

        if(input == "INT"){
            HUGO::Int i = readInt();

            m_sceneData[sceneName].entities.back().back().parameters.push_back(i);
        }

        if(input == "STRING"){
            HUGO::String s = readString();
            
            m_sceneData[sceneName].entities.back().back().parameters.push_back(s);
        }

        std::cin >> input;
    }
}

void ChatParser::handlePrefabType(){
    HUGO::String prefabName;
    std::cin >> prefabName;

    m_prefabData[prefabName] = EntityData();

    HUGO::String input;
    std::cin >> input;
    while(std::cin){
        if(input == "COMPONENT"){
            ComponentData compData;
            std::cin >> compData.componentName;

            m_prefabData[prefabName].push_back(compData);
        }

        if(input == "VECTOR_3"){
            Vector3 vec3 = readVector3();

            m_prefabData[prefabName].back().parameters.push_back(vec3);
        }

        if(input == "FLOAT"){
            HUGO::Float f = readFloat();

            m_prefabData[prefabName].back().parameters.push_back(f);
        }

        if(input == "INT"){
            HUGO::Int i = readInt();

            m_prefabData[prefabName].back().parameters.push_back(i);
        }

        if(input == "STRING"){
            HUGO::String s = readString();
            
            m_prefabData[prefabName].back().parameters.push_back(s);
        }
    }
}

EntityData ChatParser::getPrefabData(const HUGO::String& prefabName){
    std::unordered_map<HUGO::String, EntityData>::iterator res = m_prefabData.find(prefabName);

    if(res == m_prefabData.end()) {
        //No existe lo que se pide
    }

    return res->second;

}

SceneData ChatParser::getSceneData(const HUGO::String & sceneName){
    std::unordered_map<HUGO::String, SceneData>::iterator res = m_sceneData.find(sceneName);
    
    if(res == m_sceneData.end()) {
        //No existe lo que se pide
    }

    return res->second;
}

//------Read type methods---------

HUGO::Vector3 ChatParser::readVector3(){
    Vector3 res;
    std::cin >> res.x >> res.y >> res.z;

    return res;
}

HUGO::Int ChatParser::readInt(){
    HUGO::Int res;
    std::cin >> res;

    return res;
}

HUGO::Float ChatParser::readFloat(){
    HUGO::Float res;
    std::cin >> res;

    return res;
}

HUGO::String ChatParser::readString(){
    HUGO::String res;

    /*
    ...
    valueName STRING
        Hola, que tal estas
    ...
    */
    getline(std::cin, res);

    return res;
}

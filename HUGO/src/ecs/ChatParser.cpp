#include "ChatParser.h"
#include <any>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

using namespace HUGO;


ChatParser::ChatParser(): m_sceneData(), m_prefabData(){

}

ErrorOrVoid ChatParser::readChatFolder(const HUGO::String& chatFolder){

    std::filesystem::directory_iterator dirIt = std::filesystem::directory_iterator(chatFolder);

    //If dir not found or dir empty
    if(dirIt == std::filesystem::end(dirIt)){
        StringError err;
        err.errorMessage = "Folder {" + chatFolder + "} doesn't exist or is empty";

        return ErrorOrVoid(err);
    }

    //Iterate all the entries found in dir
    for(const auto& entry : std::filesystem::directory_iterator(chatFolder)) {
        HUGO::String chatPath = chatFolder + static_cast<HUGO::String>(entry.path().filename().generic_string());

        ErrorOrVoid err = readChat(chatPath);
        //If error has ben returned from reading .chat, chat parser should stop reading and return the error
        if (err.isError()){
            return err;
        }
    }

    return ErrorOrVoid();
}

ErrorOrVoid ChatParser::readChat(const HUGO::String& chatPath){
    
    
    std::ifstream file(chatPath);
    if(!file.is_open()){
        //Error opening file
        StringError err;
        err.errorMessage = "File {" + chatPath + "} can't be opened";
        return ErrorOrVoid(err);
    }

    std::streambuf* cinbuf = std::cin.rdbuf(file.rdbuf());

    
    HUGO::String chatType;

    std::cin >> chatType;

    ErrorOrVoid err = ErrorOrVoid();
    if(chatType == "SCENE"){
        err = handleSceneType();
    }
    else if(chatType == "PREFAB"){
        err = handlePrefabType();
    }
    
    std::cin.rdbuf(cinbuf);
    file.close();
    
    //If error has returned from either of the two functions, it will be returned
    return err;
}

#pragma region ReadTypeFunctions

ErrorOrType<HUGO::Vector3> readVector3(){
    Vector3 res;
    std::cin >> res.x >> res.y >> res.z;

    return ErrorOrType<HUGO::Vector3>(res);
}

ErrorOrType<HUGO::Int> readInt(){
    HUGO::Int res;
    std::cin >> res;

    return ErrorOrType<HUGO::Int>(res);
}

ErrorOrType<HUGO::Float> readFloat(){
    HUGO::Float res;
    std::cin >> res;

    return ErrorOrType<HUGO::Float>(res);
}

ErrorOrType<HUGO::String> readString(){
    HUGO::String res;

    /*
    ...
    valueName STRING
        Hola, que tal estas
    ...
    */
    getline(std::cin, res);

    return ErrorOrType<HUGO::String>(res);
}

//Checks if input is VEC3, FLOAT, INT, or STRING and adds it to the parameters of compData. If an error occurred, returns the error.
ErrorOrVoid parameterReaderHelper(const HUGO::String& input, ComponentData& compData){
    if(input == "VECTOR_3"){
            ErrorOrType<HUGO::Vector3> vec3 = readVector3();

            if(vec3.isError()) return ErrorOrVoid(vec3.getError());

            //Might be terrorism
            //Adds it to the parameters of the last component added to the last entity of the scene
            compData.parameters.push_back(vec3.getValue());
        }

        if(input == "FLOAT"){
            ErrorOrType<HUGO::Float> f = readFloat();

            if(f.isError()) return ErrorOrVoid(f.getError());

            compData.parameters.push_back(f.getValue());
        }

        if(input == "INT"){
            ErrorOrType<HUGO::Int> i = readInt();

            if(i.isError()) return ErrorOrVoid(i.getError());

            compData.parameters.push_back(i.getValue());
        }

        if(input == "STRING"){
            ErrorOrType<HUGO::String> s = readString();

            if(s.isError()) return ErrorOrVoid(s.getError());
            
           compData.parameters.push_back(s.getValue());
        }

    return ErrorOrVoid();
}

#pragma endregion

ErrorOrVoid ChatParser::handleSceneType(){
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

        ErrorOrVoid parameterReadErr = parameterReaderHelper(input, m_sceneData[sceneName].entities.back().back());
        if(parameterReadErr.isError()) return parameterReadErr;

        std::cin >> input;
    }

    return ErrorOrVoid();
}

ErrorOrVoid ChatParser::handlePrefabType(){
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

        ErrorOrVoid parameterReadErr = parameterReaderHelper(input, m_prefabData[prefabName].back());
        if(parameterReadErr.isError()) return parameterReadErr;

        std::cin >> input;
    }

    return ErrorOrVoid();
}

ErrorOrType<EntityData> ChatParser::getPrefabData(const HUGO::String& prefabName){
    std::unordered_map<HUGO::String, EntityData>::iterator res = m_prefabData.find(prefabName);

    if(res == m_prefabData.end()) {
        //No existe lo que se pide
        StringError error = StringError();
        error.errorMessage = "Queried prefab doesn't exist";
        return ErrorOrType<EntityData>(error);
    }

    return ErrorOrType<EntityData>(res->second);

}

ErrorOrType<SceneData> ChatParser::getSceneData(const HUGO::String & sceneName){
    std::unordered_map<HUGO::String, SceneData>::iterator res = m_sceneData.find(sceneName);
    
    if(res == m_sceneData.end()) {
        //No existe lo que se pide
        StringError error = StringError();
        error.errorMessage = "Queried scene doesn't exist";
        return ErrorOrType<SceneData>(error);
    }

    return ErrorOrType<SceneData>(res->second);
}

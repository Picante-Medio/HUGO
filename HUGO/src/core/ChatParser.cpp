#include "ChatParser.h"
#include "HugoError.h"
#include "HugoTypes.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace HUGO;


ChatParser::ChatParser(): m_sceneData(), m_prefabData(){

}

ErrorOrVoid ChatParser::readChatFolder(const HUGO::String& chatFolder){

    std::filesystem::path chatFolderPath{chatFolder};
    chatFolderPath = std::filesystem::current_path()/chatFolderPath;

    //If dir not found or dir empty
    if(!std::filesystem::exists(chatFolderPath))
        return ErrorOrVoid(StringError{HUGO::String("Folder {" + chatFolderPath.string() + "} doesn't exist")});

    std::filesystem::directory_iterator dirIt = std::filesystem::directory_iterator(chatFolderPath);

    //Iterate all the entries found in dir
    for(const auto& entry : dirIt) {

        std::filesystem::path chatPath = chatFolderPath/entry.path();
        ErrorOrVoid err = readChat(chatPath.string());
        //If error has ben returned from reading .chat, chat parser should stop reading and return the error
        if (err.isError()){
            HUGO::String message = "Error reading .chat in path {" + chatPath.filename().string()+ "}: " + err.getError().description();
            return ErrorOrVoid(StringError{message});
        }
    }

    return ErrorOrVoid();
}

ErrorOrVoid ChatParser::readChat(const HUGO::String& chatPath){
    std::ifstream file(chatPath);
    if(!file.is_open())
        return ErrorOrVoid(StringError{HUGO::String("File can't be opened")});

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
    if (std::cin >> res.x >> res.y >> res.z) return ErrorOrType<HUGO::Vector3>(res);
    else return ErrorOrType<HUGO::Vector3>(StringError{HUGO::String("Error reading VECTOR_3")});
}

ErrorOrType<HUGO::Int> readInt(){
    HUGO::Int res;
    if (std::cin >> res) return ErrorOrType<HUGO::Int>(res);

    StringError err;
    err.errorMessage = "Error reading INT";
    return ErrorOrType<HUGO::Int>(err);
}

ErrorOrType<HUGO::Float> readFloat(){
    HUGO::Float res;
    if (std::cin >> res) return ErrorOrType<HUGO::Float>(res);
    else return ErrorOrType<HUGO::Float>(StringError{HUGO::String("Error reading FLOAT")});
}

ErrorOrType<HUGO::String> readString(){
    HUGO::String res;
    if (getline(std::cin, res)) return ErrorOrType<HUGO::String>(res);
    else return ErrorOrType<HUGO::String>("Error reading STRING");
}

//Checks if input is VEC3, FLOAT, INT, or STRING and adds it to the parameters of compData. If an error occurred, returns the error.
ErrorOrVoid parameterReaderHelper(const HUGO::String& input, ComponentData& compData){
    if(input == "VECTOR_3"){

        HUGO::String parameterName;
        std::cin >> parameterName;

            ErrorOrType<HUGO::Vector3> vec3 = readVector3();

            if(vec3.isError()) return ErrorOrVoid(vec3.getError());

            //Might be terrorism
            //Adds it to the parameters of the last component added to the last entity of the scene
            compData.parameters[parameterName] = {vec3.getValue()};
        }

        if(input == "FLOAT"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            ErrorOrType<HUGO::Float> f = readFloat();

            if(f.isError()) return ErrorOrVoid(f.getError());

            compData.parameters[parameterName] = {f.getValue()};
        }

        if(input == "INT"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            ErrorOrType<HUGO::Int> i = readInt();

            if(i.isError()) return ErrorOrVoid(i.getError());

            compData.parameters[parameterName] = {i.getValue()};
        }

        if(input == "STRING"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            std::cin.get();
            ErrorOrType<HUGO::String> s = readString();

            if(s.isError()) return ErrorOrVoid(s.getError());

           compData.parameters[parameterName] = {s.getValue()};
        }

    return ErrorOrVoid();
}

#pragma endregion

ErrorOrVoid ChatParser::handleSceneType(){
    std::cin.get();

    HUGO::String sceneName;
    getline(std::cin, sceneName);

    //Initialize scene data of name sceneName
    m_sceneData[sceneName].entities = std::vector<EntityData>();
    m_sceneData[sceneName].systems = std::vector<HUGO::String>();
    m_sceneData[sceneName].resources = std::vector<HUGO::String>();

    HUGO::String input;
    std::cin >> input;

    while(std::cin){

        if(input == "SYSTEM"){
            HUGO::String systemName;
            std::cin >> systemName;

            m_sceneData[sceneName].systems.push_back(systemName);
        }

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

        //If it already has an entity and this entity has a component, it can check if possible parameters for the component have been read.
        if(m_sceneData[sceneName].entities.size() > 0 && m_sceneData[sceneName].entities.back().size() > 0){
            ErrorOrVoid parameterReadErr = parameterReaderHelper(input, m_sceneData[sceneName].entities.back().back());
            if(parameterReadErr.isError()) return parameterReadErr;
        }

        std::cin >> input;
    }

    return ErrorOrVoid();
}

ErrorOrVoid ChatParser::handlePrefabType(){
    char emptySpaceChar;
    std::cin >> emptySpaceChar;

    HUGO::String prefabName;
    getline(std::cin, prefabName);

    HUGO::Int prefabID;
    std::cin >> prefabID;

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

    if(res == m_prefabData.end()) 
        return ErrorOrType<EntityData>(StringError{HUGO::String("Queried prefab doesn't exist")});

    return ErrorOrType<EntityData>(res->second);
}

ErrorOrType<SceneData> ChatParser::getSceneData(const HUGO::String & sceneName){
    std::unordered_map<HUGO::String, SceneData>::iterator res = m_sceneData.find(sceneName);
    if(res == m_sceneData.end()) 
        return ErrorOrType<SceneData>(StringError{HUGO::String("Queried scene named " + sceneName + " doesn't exist")});

    return ErrorOrType<SceneData>(res->second);
}

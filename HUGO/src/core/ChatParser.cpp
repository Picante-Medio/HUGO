#include "ChatParser.h"
#include "HugoError.h"
#include "HugoTypes.h"

#include <iostream>
#include <fstream>
#include <filesystem>

using namespace HUGO;


ChatParser::ChatParser(): m_sceneData(), m_prefabData(){

}

HUGO::ErrorOrVoid ChatParser::readChatFolder(const HUGO::String& chatFolder){

    std::filesystem::path chatFolderPath{chatFolder};
    chatFolderPath = std::filesystem::current_path()/chatFolderPath;

    //If dir not found or dir empty
    if(!std::filesystem::exists(chatFolderPath))
        return HUGO::ErrorOrVoid(HUGO::StringError{HUGO::String("Folder {" + chatFolderPath.string() + "} doesn't exist")});

    std::filesystem::directory_iterator dirIt = std::filesystem::directory_iterator(chatFolderPath);

    //Iterate all the entries found in dir
    for(const auto& entry : dirIt) {

        std::filesystem::path chatPath = chatFolderPath/entry.path();
        HUGO::ErrorOrVoid err = readChat(chatPath.string());
        //If error has ben returned from reading .chat, chat parser should stop reading and return the error
        if (err.isError()){
            HUGO::String message = "Error reading .chat in path {" + chatPath.filename().string()+ "}: " + err.getError().description();
            return HUGO::ErrorOrVoid(HUGO::StringError{message});
        }
    }

    return HUGO::ErrorOrVoid();
}

HUGO::ErrorOrVoid ChatParser::readChat(const HUGO::String& chatPath){
    std::ifstream file(chatPath);
    if(!file.is_open())
        return HUGO::ErrorOrVoid(HUGO::StringError{HUGO::String("File can't be opened")});

    std::streambuf* cinbuf = std::cin.rdbuf(file.rdbuf());

    HUGO::String chatType;

    std::cin >> chatType;

    HUGO::ErrorOrVoid err = HUGO::ErrorOrVoid();
    if(chatType == "SCENE"){
        err = handleSceneType();
    }
    else if(chatType == "PREFAB"){
        err = handlePrefabType();
    }
    else return HUGO::ErrorOrVoid(HUGO::StringError{"Chat file is neither a scene or a prefab"});

    std::cin.rdbuf(cinbuf);
    file.close();

    //If error has returned from either of the two functions, it will be returned
    return err;
}

#pragma region ReadTypeFunctions

HUGO::ErrorOrType<HUGO::Vector2> readVector2(){
    Vector2 res;
    if (std::cin >> res.x >> res.y) return HUGO::ErrorOrType<HUGO::Vector2>(res);
    else return HUGO::ErrorOrType<HUGO::Vector2>(HUGO::StringError{HUGO::String("Error reading VECTOR_2")});
}

HUGO::ErrorOrType<HUGO::Vector3> readVector3(){
    Vector3 res;
    if (std::cin >> res.x >> res.y >> res.z) return HUGO::ErrorOrType<HUGO::Vector3>(res);
    else return HUGO::ErrorOrType<HUGO::Vector3>(HUGO::StringError{HUGO::String("Error reading VECTOR_3")});
}

HUGO::ErrorOrType<HUGO::Vector4> readVector4(){
    Vector4 res;
    if (std::cin >> res.x >> res.y >> res.z >> res.w) return HUGO::ErrorOrType<HUGO::Vector4>(res);
    else return HUGO::ErrorOrType<HUGO::Vector4>(HUGO::StringError{HUGO::String("Error reading VECTOR_4")});
}

HUGO::ErrorOrType<HUGO::Quaternion> readQuaternion(){
    HUGO::ErrorOrType<HUGO::Vector4> vec4 = readVector4();
    if(!vec4.isError()) return HUGO::ErrorOrType<HUGO::Quaternion>(vec4.getValue());
    else return HUGO::ErrorOrType<HUGO::Quaternion>(HUGO::StringError{HUGO::String("Error reading QUATERNION")});
}



HUGO::ErrorOrType<HUGO::Int> readInt(){
    HUGO::Int res;
    if (std::cin >> res) return HUGO::ErrorOrType<HUGO::Int>(res);

    HUGO::StringError err;
    err.errorMessage = "Error reading INT";
    return HUGO::ErrorOrType<HUGO::Int>(err);
}

HUGO::ErrorOrType<HUGO::Float> readFloat(){
    HUGO::Float res;
    if (std::cin >> res) return HUGO::ErrorOrType<HUGO::Float>(res);
    else return HUGO::ErrorOrType<HUGO::Float>(HUGO::StringError{HUGO::String("Error reading FLOAT")});
}

HUGO::ErrorOrType<HUGO::Double> readDouble(){
    HUGO::Double res;
    if (std::cin >> res) return HUGO::ErrorOrType<HUGO::Double>(res);
    else return HUGO::ErrorOrType<HUGO::Double>(HUGO::StringError{HUGO::String("Error reading DOUBLE")});
}

HUGO::ErrorOrType<HUGO::String> readString(){
    HUGO::String res;
    if (getline(std::cin, res)) return HUGO::ErrorOrType<HUGO::String>(res);
    else return HUGO::ErrorOrType<HUGO::String>("Error reading STRING");
}

HUGO::ErrorOrType<HUGO::Bool> readBool(){
    HUGO::String res;
    if (std::cin >> res) {
        if(res == "true") return HUGO::ErrorOrType<HUGO::Bool>(true);
        if(res == "false") return HUGO::ErrorOrType<HUGO::Bool>(false);
    }
    return HUGO::ErrorOrType<HUGO::Bool>("Error reading BOOL");
}

HUGO::ErrorOrType <HUGO::ColliderInfo> readColliderInfo(){
    HUGO::ColliderInfo res;

    HUGO::String col_shape;
    std::cin >> col_shape;

    if(col_shape == "BOX_COLLIDER"){
        //vec3 half_extents
        HUGO::ErrorOrType<Vector3> half_extents = readVector3();
        if(half_extents.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(half_extents.getError());
        res = BoxColliderInfo{half_extents.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "SPHERE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<Float> radius = readFloat();
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());
        res = SphereColliderInfo{radius.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CAPSULE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<Float> radius = readFloat();
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());

        //float height
        HUGO::ErrorOrType<Float> height = readFloat();
        if(height.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(height.getError());
        res = CapsuleColliderInfo{radius.getValue(), height.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CYLINDER_COLLIDER"){
        //vec3 half_extents
        HUGO::ErrorOrType<Vector3> half_extents = readVector3();
        if(half_extents.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(half_extents.getError());
        res = CylinderColliderInfo{half_extents.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CONE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<Float> radius = readFloat();
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());

        //float height
        HUGO::ErrorOrType<Float> height = readFloat();
        if(height.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(height.getError());
        res = ConeColliderInfo{radius.getValue(), height.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    return HUGO::ErrorOrType<HUGO::ColliderInfo>(HUGO::StringError{"Error reading ColliderInfo"});
}

//Checks if input is BOOL, VEC3, FLOAT, INT, STRING... and adds it to the parameters of compData. If an error occurred, returns the error.
HUGO::ErrorOrVoid parameterReaderHelper(const HUGO::String& input, ComponentData& compData){
        if(input == "VECTOR_2"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector2> vec2 = readVector2();

            if(vec2.isError()) return HUGO::ErrorOrVoid(vec2.getError());
            compData.parameters[parameterName] = {vec2.getValue()};
        }

        if(input == "VECTOR_3"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector3> vec3 = readVector3();

            if(vec3.isError()) return HUGO::ErrorOrVoid(vec3.getError());
            compData.parameters[parameterName] = {vec3.getValue()};
        }

        if(input == "VECTOR_4"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector4> vec4 = readVector4();

            if(vec4.isError()) return HUGO::ErrorOrVoid(vec4.getError());
            compData.parameters[parameterName] = {vec4.getValue()};
        }

        if(input == "QUATERNION"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Quaternion> quat = readQuaternion();

            if(quat.isError()) return HUGO::ErrorOrVoid(quat.getError());
            compData.parameters[parameterName] = {quat.getValue()};
        }

        if(input == "FLOAT"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Float> f = readFloat();

            if(f.isError()) return HUGO::ErrorOrVoid(f.getError());

            compData.parameters[parameterName] = {f.getValue()};
        }

        if(input == "DOUBLE"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Double> d = readDouble();

            if(d.isError()) return HUGO::ErrorOrVoid(d.getError());

            compData.parameters[parameterName] = {d.getValue()};
        }

        if(input == "INT"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            HUGO::ErrorOrType<HUGO::Int> i = readInt();

            if(i.isError()) return HUGO::ErrorOrVoid(i.getError());

            compData.parameters[parameterName] = {i.getValue()};
        }

        if(input == "STRING"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            std::cin.get();
            HUGO::ErrorOrType<HUGO::String> s = readString();

            if(s.isError()) return HUGO::ErrorOrVoid(s.getError());

           compData.parameters[parameterName] = {s.getValue()};
        }

        if(input == "BOOL"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            std::cin.get();
            HUGO::ErrorOrType<HUGO::Bool> b = readBool();

            if(b.isError()) return HUGO::ErrorOrVoid(b.getError());

           compData.parameters[parameterName] = {b.getValue()};
        }

        if(input == "COLLIDER_INFO"){
            HUGO::String parameterName;
            std::cin >> parameterName;

            std::cin.get();
            HUGO::ErrorOrType<HUGO::ColliderInfo> b = readColliderInfo();
            if(b.isError()) return HUGO::ErrorOrVoid(b.getError());

           compData.parameters[parameterName] = {b.getValue()};
        }

    return HUGO::ErrorOrVoid();
}

#pragma endregion

HUGO::ErrorOrVoid ChatParser::handleSceneType(){
    std::cin.get();

    HUGO::String sceneName;
    getline(std::cin, sceneName);

    //Initialize scene data of name sceneName
    m_sceneData[sceneName].entities = std::vector<EntityData>();
    m_sceneData[sceneName].systems = std::vector<HUGO::String>();

    HUGO::String input;
    std::cin >> input;

    while(std::cin){

        if(input == "SYSTEM"){
            HUGO::String systemName;
            std::cin >> systemName;

            m_sceneData[sceneName].systems.push_back(systemName);
        }

        if (input == "ENTITY"){
            HUGO::ErrorOrType<EntityData> entData = handleEntity();
            if(entData.isError()){
                return HUGO::ErrorOrVoid(entData.getError());
            }

            m_sceneData[sceneName].entities.push_back(entData.getValue());
        }

        std::cin >> input;
    }

    return HUGO::ErrorOrVoid();
}

HUGO::ErrorOrVoid ChatParser::handlePrefabType(){
    
    HUGO::String prefabName;
    std::cin >> prefabName;

    HUGO::String input;
    std::cin >> input;

    if(input != "ENTITY"){
        return HUGO::ErrorOrVoid(HUGO::StringError{"Bad format in prefab .chat"});
    }

    HUGO::ErrorOrType<EntityData> entData = handleEntity();

    if(entData.isError()) return HUGO::ErrorOrVoid(entData.getError());

    m_prefabData[prefabName] = entData.getValue();

    return HUGO::ErrorOrVoid();

}

HUGO::ErrorOrType<EntityData> ChatParser::handleEntity(){
    EntityData entityData;
    HUGO::String entityName;
    HUGO::Int entityID;
    std::cin >> entityName >> entityID;

    HUGO::String input;
    std::cin >> input;
    while(input != "END_ENTITY"){
        if(input == "COMPONENT"){
            ComponentData compData;
            std::cin >> compData.componentName;
            entityData.components.push_back(compData);
        }
        else if(input == "START_CHILDREN"){
            std::cin >> input;
            while(input != "END_CHILDREN"){
                HUGO::ErrorOrType<EntityData> childEnt = handleEntity();
                if(childEnt.isError()) return HUGO::ErrorOrType<EntityData>(childEnt.getError());
                entityData.children.push_back(childEnt.getValue());

                std::cin >> input;
            }
        }
        //If it already has an entity and this entity has a component, it can check if possible parameters for the component have been read.
        else if(entityData.components.size() > 0){
            HUGO::ErrorOrVoid parameterReadErr = parameterReaderHelper(input, entityData.components.back());
            if(parameterReadErr.isError()) return HUGO::ErrorOrType<EntityData>(parameterReadErr.getError());
        }

        std::cin >> input;
    }

    return HUGO::ErrorOrType<EntityData>(entityData);
}

HUGO::ErrorOrType<EntityData> ChatParser::getPrefabData(const HUGO::String& prefabName){
    std::unordered_map<HUGO::String, EntityData>::iterator res = m_prefabData.find(prefabName);

    if(res == m_prefabData.end()) 
        return HUGO::ErrorOrType<EntityData>(HUGO::StringError{HUGO::String("Queried prefab doesn't exist")});

    return HUGO::ErrorOrType<EntityData>(res->second);
}

HUGO::ErrorOrType<SceneData> ChatParser::getSceneData(const HUGO::String & sceneName){
    std::unordered_map<HUGO::String, SceneData>::iterator res = m_sceneData.find(sceneName);
    if(res == m_sceneData.end()) 
        return HUGO::ErrorOrType<SceneData>(HUGO::StringError{HUGO::String("Queried scene named " + sceneName + " doesn't exist")});

    return HUGO::ErrorOrType<SceneData>(res->second);
}

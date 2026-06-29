#include "ChatParser.h"
#include "HugoError.h"
#include "HugoTypes.h"
#include "TypeFactory.h"

#include <iostream>
#include <fstream>


using namespace HUGO;


ChatParser::ChatParser(): m_sceneData(), m_prefabData(){

}

void ChatParser::readChatFolder(const std::string& chatFolder){
    m_chatFolderPath = std::filesystem::current_path()/m_chatFolderPath;

    m_chatFolderPath = {chatFolder};
    //If dir not found or dir empty
    if(!std::filesystem::exists(m_chatFolderPath))
        throw std::runtime_error("Folder {" + m_chatFolderPath.string() + "} doesn't exist");

    std::filesystem::directory_iterator dirIt = std::filesystem::directory_iterator(m_chatFolderPath);

    //Iterate all the entries found in dir
    for(const auto& entry : dirIt) {

        std::filesystem::path chatPath = m_chatFolderPath/entry.path();
        HUGO::ErrorOrVoid err = readPrefab(chatPath.string());
        //If error has been returned from reading .chat, chat parser should stop reading and return the error
        if (err.isError()){
            throw std::runtime_error(
                "Error reading .chat in path {" + chatPath.filename().string()+ "}: " + err.getError().description());
        }
    }
}

HUGO::ErrorOrVoid ChatParser::readPrefab(const std::string& chatPath){
    std::ifstream file(chatPath);
    if(!file.is_open())
        return HUGO::ErrorOrVoid(StringError{std::string("File can't be opened")});

    std::string chatType;

    file >> chatType;

    HUGO::ErrorOrVoid err = HUGO::ErrorOrVoid();
    if(chatType == "PREFAB"){
        err = handlePrefabType(file);
    }
    else if(chatType != "SCENE") return HUGO::ErrorOrVoid(StringError{"Chat file is neither a scene or a prefab"});

    file.close();

    //If error has returned from either of the two functions, it will be returned
    return err;
}

HUGO::ErrorOrType<SceneData> ChatParser::readScene(const std::string& chatPath, std::string& sceneName, const std::string& target){
    std::ifstream file(chatPath);
    if(!file.is_open())
        return HUGO::ErrorOrType<SceneData>(StringError{std::string("File can't be opened")});

    std::string chatType;

    file >> chatType;

    file.get();
    getline(file, sceneName);

    HUGO::ErrorOrType<SceneData> err = HUGO::ErrorOrType<SceneData>(SceneData());

    if(chatType == "SCENE" && sceneName == target){
        err = handleSceneType(file, sceneName);
    }
    //else if(chatType != "SCENE" || chatType != "PREFAB") err = HUGO::ErrorOrType<SceneData>(StringError{"Chat file is not a scene"});

    file.close();

    //If error has returned from either of the two functions, it will be returned
    return err;
}

#pragma region ReadTypeFunctions

HUGO::ErrorOrType<HUGO::Vector2> ChatParser::readVector2(std::ifstream& file){
    Vector2 res;
    if (file >> res.x >> res.y) return HUGO::ErrorOrType<HUGO::Vector2>(res);
    else return HUGO::ErrorOrType<HUGO::Vector2>(StringError{std::string("Error reading VECTOR_2")});
}

HUGO::ErrorOrType<HUGO::Vector3> ChatParser::readVector3(std::ifstream& file){
    Vector3 res;
    if (file >> res.x >> res.y >> res.z) return HUGO::ErrorOrType<HUGO::Vector3>(res);
    else return HUGO::ErrorOrType<HUGO::Vector3>(StringError{std::string("Error reading VECTOR_3")});
}

HUGO::ErrorOrType<HUGO::Vector4> ChatParser::readVector4(std::ifstream& file){
    Vector4 res;
    if (file >> res.x >> res.y >> res.z >> res.w) return HUGO::ErrorOrType<HUGO::Vector4>(res);
    else return HUGO::ErrorOrType<HUGO::Vector4>(StringError{std::string("Error reading VECTOR_4")});
}

HUGO::ErrorOrType<HUGO::Quaternion> ChatParser::readQuaternion(std::ifstream& file){
    HUGO::ErrorOrType<HUGO::Vector4> vec4 = readVector4(file);
    if(!vec4.isError()) return HUGO::ErrorOrType<HUGO::Quaternion>(vec4.getValue());
    else return HUGO::ErrorOrType<HUGO::Quaternion>(StringError{std::string("Error reading QUATERNION")});
}



HUGO::ErrorOrType<HUGO::Int> ChatParser::readInt(std::ifstream& file){
    HUGO::Int res;
    if (file >> res) return HUGO::ErrorOrType<HUGO::Int>(res);

    StringError err;
    err.errorMessage = "Error reading INT";
    return HUGO::ErrorOrType<HUGO::Int>(err);
}

HUGO::ErrorOrType<float> ChatParser::readFloat(std::ifstream& file){
    float res;
    if (file >> res) return HUGO::ErrorOrType<float>(res);
    else return HUGO::ErrorOrType<float>(StringError{std::string("Error reading FLOAT")});
}

HUGO::ErrorOrType<double> ChatParser::readDouble(std::ifstream& file){
    double res;
    if (file >> res) return HUGO::ErrorOrType<double>(res);
    else return HUGO::ErrorOrType<double>(StringError{std::string("Error reading DOUBLE")});
}

HUGO::ErrorOrType<std::string> ChatParser::readString(std::ifstream& file){
    std::string res;
    if (getline(file, res)) return HUGO::ErrorOrType<std::string>(res);
    else return HUGO::ErrorOrType<std::string>("Error reading STRING");
}

HUGO::ErrorOrType<bool> ChatParser::readBool(std::ifstream& file){
    std::string res;
    if (file >> res) {
        if(res == "true") return HUGO::ErrorOrType<bool>(true);
        if(res == "false") return HUGO::ErrorOrType<bool>(false);
    }
    return HUGO::ErrorOrType<bool>("Error reading BOOL");
}

HUGO::ErrorOrType <HUGO::ColliderInfo> ChatParser::readColliderInfo(std::ifstream& file){
    HUGO::ColliderInfo res;

    std::string col_shape;
    file >> col_shape;

    if(col_shape == "BOX_COLLIDER"){
        //vec3 half_extents
        HUGO::ErrorOrType<Vector3> half_extents = readVector3(file);
        if(half_extents.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(half_extents.getError());
        res = BoxColliderInfo{half_extents.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "SPHERE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<float> radius = readFloat(file);
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());
        res = SphereColliderInfo{radius.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CAPSULE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<float> radius = readFloat(file);
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());

        //float height
        HUGO::ErrorOrType<float> height = readFloat(file);
        if(height.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(height.getError());
        res = CapsuleColliderInfo{radius.getValue(), height.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CYLINDER_COLLIDER"){
        //vec3 half_extents
        HUGO::ErrorOrType<Vector3> half_extents = readVector3(file);
        if(half_extents.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(half_extents.getError());
        res = CylinderColliderInfo{half_extents.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    if(col_shape == "CONE_COLLIDER"){
        //float radius
        HUGO::ErrorOrType<float> radius = readFloat(file);
        if(radius.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(radius.getError());

        //float height
        HUGO::ErrorOrType<float> height = readFloat(file);
        if(height.isError()) return HUGO::ErrorOrType<HUGO::ColliderInfo>(height.getError());
        res = ConeColliderInfo{radius.getValue(), height.getValue()};

        return HUGO::ErrorOrType<HUGO::ColliderInfo>(res);
    }

    return HUGO::ErrorOrType<HUGO::ColliderInfo>(StringError{"Error reading ColliderInfo"});
}

//Checks if input is BOOL, VEC3, FLOAT, INT, STRING... and adds it to the parameters of compData. If an error occurred, returns the error.
HUGO::ErrorOrVoid ChatParser::parameterReaderHelper(const std::string& input, ComponentData& compData, std::ifstream& file){
        if(input == "VECTOR_2"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector2> vec2 = readVector2(file);

            if(vec2.isError()) return HUGO::ErrorOrVoid(vec2.getError());
            compData.parameters[parameterName] = {vec2.getValue()};
        }

        if(input == "VECTOR_3"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector3> vec3 = readVector3(file);

            if(vec3.isError()) return HUGO::ErrorOrVoid(vec3.getError());
            compData.parameters[parameterName] = {vec3.getValue()};
        }

        if(input == "VECTOR_4"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<HUGO::Vector4> vec4 = readVector4(file);

            if(vec4.isError()) return HUGO::ErrorOrVoid(vec4.getError());
            compData.parameters[parameterName] = {vec4.getValue()};
        }

        if(input == "QUATERNION"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<HUGO::Quaternion> quat = readQuaternion(file);

            if(quat.isError()) return HUGO::ErrorOrVoid(quat.getError());
            compData.parameters[parameterName] = {quat.getValue()};
        }

        if(input == "FLOAT"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<float> f = readFloat(file);

            if(f.isError()) return HUGO::ErrorOrVoid(f.getError());

            compData.parameters[parameterName] = {f.getValue()};
        }

        if(input == "DOUBLE"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<double> d = readDouble(file);

            if(d.isError()) return HUGO::ErrorOrVoid(d.getError());

            compData.parameters[parameterName] = {d.getValue()};
        }

        if(input == "INT"){
            std::string parameterName;
            file >> parameterName;

            HUGO::ErrorOrType<HUGO::Int> i = readInt(file);

            if(i.isError()) return HUGO::ErrorOrVoid(i.getError());

            compData.parameters[parameterName] = {i.getValue()};
        }

        if(input == "STRING"){
            std::string parameterName;
            file >> parameterName;

            file.get();
            HUGO::ErrorOrType<std::string> s = readString(file);

            if(s.isError()) return HUGO::ErrorOrVoid(s.getError());

           compData.parameters[parameterName] = {s.getValue()};
        }

        if(input == "BOOL"){
            std::string parameterName;
            file >> parameterName;

            file.get();
            HUGO::ErrorOrType<bool> b = readBool(file);

            if(b.isError()) return HUGO::ErrorOrVoid(b.getError());

           compData.parameters[parameterName] = {b.getValue()};
        }

        if(input == "COLLIDER_INFO"){
            std::string parameterName;
            file >> parameterName;

            file.get();
            HUGO::ErrorOrType<HUGO::ColliderInfo> b = readColliderInfo(file);
            if(b.isError()) return HUGO::ErrorOrVoid(b.getError());

           compData.parameters[parameterName] = {b.getValue()};
        }

        if(TypeFactory::IsRegistered(input)) {
            std::string parameterName;
            file >> parameterName;

            std::string args;
            file >> args;

            compData.parameters[parameterName] = {TypeFactory::Create(input, args)};
        }

    return HUGO::ErrorOrVoid();
}

#pragma endregion

HUGO::ErrorOrType<SceneData> ChatParser::handleSceneType(std::ifstream& file, const std::string& sceneName){

    //Initialize scene data of name sceneName
    SceneData data;
    data.entities = std::vector<EntityData>();
    data.systems = std::vector<std::string>();

    std::string input;
    file >> input;

    while(file){

        if(input == "SYSTEM"){
            std::string systemName;
            file >> systemName;

            data.systems.push_back(systemName);
        }

        if (input == "ENTITY"){
            HUGO::ErrorOrType<EntityData> entData = handleEntity(file);
            if(entData.isError()){
                return HUGO::ErrorOrType<SceneData>(entData.getError());
            }

            data.entities.push_back(entData.getValue());
        }

        file >> input;
    }
    m_sceneData[sceneName] = data;
    return HUGO::ErrorOrType<SceneData>(data);
}

HUGO::ErrorOrVoid ChatParser::handlePrefabType(std::ifstream& file){
    
    std::string prefabName;
    file >> prefabName;

    std::string input;
    file >> input;

    if(input != "ENTITY"){
        return HUGO::ErrorOrVoid(StringError{"Bad format in prefab .chat"});
    }

    HUGO::ErrorOrType<EntityData> entData = handleEntity(file);

    if(entData.isError()) return HUGO::ErrorOrVoid(entData.getError());

    m_prefabData[prefabName] = entData.getValue();

    return HUGO::ErrorOrVoid();

}

HUGO::ErrorOrType<EntityData> ChatParser::handleEntity(std::ifstream& file){
    EntityData entityData;
    std::string entityName;
    HUGO::Int entityID;
    file >> entityName >> entityID;

    std::string input;
    file >> input;
    while(input != "END_ENTITY"){
        if(input == "COMPONENT"){
            ComponentData compData;
            file >> compData.componentName;
            entityData.components.push_back(compData);
        }
        else if(input == "START_CHILDREN"){
            file >> input;
            while(input != "END_CHILDREN"){
                HUGO::ErrorOrType<EntityData> childEnt = handleEntity(file);
                if(childEnt.isError()) return HUGO::ErrorOrType<EntityData>(childEnt.getError());
                entityData.children.push_back(childEnt.getValue());

                file >> input;
            }
        }
        //If it already has an entity and this entity has a component, it can check if possible parameters for the component have been read.
        else if(entityData.components.size() > 0){
            HUGO::ErrorOrVoid parameterReadErr = parameterReaderHelper(input, entityData.components.back(), file);
            if(parameterReadErr.isError()) return HUGO::ErrorOrType<EntityData>(parameterReadErr.getError());
        }

        file >> input;
    }

    return HUGO::ErrorOrType<EntityData>(entityData);
}

HUGO::ErrorOrType<EntityData> ChatParser::getPrefabData(const std::string& prefabName){
    std::unordered_map<std::string, EntityData>::iterator res = m_prefabData.find(prefabName);

    if(res == m_prefabData.end()) 
        return HUGO::ErrorOrType<EntityData>(StringError{std::string("Queried prefab doesn't exist")});

    return HUGO::ErrorOrType<EntityData>(res->second);
}

HUGO::ErrorOrType<SceneData> ChatParser::getSceneData(const std::string & sceneName){
    
    std::unordered_map<std::string, SceneData>::iterator res = m_sceneData.find(sceneName);
    if(res == m_sceneData.end()) 
        return findScene(sceneName);

    return HUGO::ErrorOrType<SceneData>(res->second);
}

HUGO::ErrorOrType<SceneData> ChatParser::findScene(const std::string& target){
    //If dir not found or dir empty
    if(!std::filesystem::exists(m_chatFolderPath))
        throw std::runtime_error("Folder {" + m_chatFolderPath.string() + "} doesn't exist");

    std::filesystem::directory_iterator dirIt = std::filesystem::directory_iterator(m_chatFolderPath);

    //Iterate all the entries found in dir
    for(const auto& entry : dirIt) {

        std::filesystem::path chatPath = m_chatFolderPath/entry.path();
        std::string sceneName;
        HUGO::ErrorOrType<SceneData> err = readScene(chatPath.string(), sceneName, target);

        if(sceneName == target) return err;

        //If error has been returned from reading .chat, chat parser should stop reading and return the error
        if (err.isError()){
            throw std::runtime_error(
                "Error reading .chat in path {" + chatPath.filename().string()+ "}: " + err.getError().description());
        }
    }

    return HUGO::ErrorOrType<SceneData>(HUGO::StringError("Scene called "+ target + " doesn't exist"));
}

#pragma once

#include <unordered_map>
#include <vector>
#include <HugoMath.h>
#include <filesystem>
#include "HugoError.h"
#include "HugoParsingVariant.h"


namespace HUGO{
	struct ComponentData {
		std::string componentName;
		std::unordered_map<std::string, HVariant> parameters;
	};

	struct EntityData{
		std::vector<ComponentData> components;
		std::vector<EntityData> children;
	};

	struct SceneData {
		
		std::vector<EntityData> entities;
		std::vector<std::string> systems;
	};

	class ChatParser {
	private:
		
		//Data that can be queried
		std::unordered_map<std::string, SceneData> m_sceneData;
		std::unordered_map<std::string, EntityData> m_prefabData;
		std::filesystem::path m_chatFolderPath;

		//Open .chat file and read it
		HUGO::ErrorOrVoid readPrefab(const std::string& chatPath);
		HUGO::ErrorOrType<SceneData> readScene(const std::string& chatPath, std::string& sceneName, const std::string& target);

		//Read previously opened file as Scene.
		HUGO::ErrorOrType<SceneData> handleSceneType(std::ifstream& file, const std::string& sceneName);

		//Reads Entity until it reads END_ENTITY
		HUGO::ErrorOrType<EntityData> handleEntity(std::ifstream& file);

		//Read previously opened file as Prefab
		HUGO::ErrorOrVoid handlePrefabType(std::ifstream& file);

		//Read parameter types
		HUGO::ErrorOrType<HUGO::Vector2> readVector2(std::ifstream& file);
		HUGO::ErrorOrType<HUGO::Vector3> readVector3(std::ifstream& file);
		HUGO::ErrorOrType<HUGO::Vector4> readVector4(std::ifstream& file);
		HUGO::ErrorOrType<HUGO::Quaternion> readQuaternion(std::ifstream& file);
		HUGO::ErrorOrType<HUGO::Int> readInt(std::ifstream& file);
		HUGO::ErrorOrType<float> readFloat(std::ifstream& file);
		HUGO::ErrorOrType<double> readDouble(std::ifstream& file);
		HUGO::ErrorOrType<std::string> readString(std::ifstream& file);
		HUGO::ErrorOrType<bool> readBool(std::ifstream& file);
		HUGO::ErrorOrType <HUGO::ColliderInfo> readColliderInfo(std::ifstream& file);

		//Helper function to read parameter
		HUGO::ErrorOrVoid parameterReaderHelper(const std::string& input, ComponentData& compData, std::ifstream& file);
		
		//Search for a scene by name
		HUGO::ErrorOrType<SceneData> findScene(const std::string& target);

	public:

		//Initialize empty database. Function readChatFolder must be called to fill the database.
		ChatParser();

		//Adds to the database the data from all .chat files of chatFolder path.
		//@param chatFolder Path relative to the executable file (normally HUGO/bin)
		void readChatFolder(const std::string& chatFolder);

		//Returns entity data associated with prefabName
		HUGO::ErrorOrType<EntityData> getPrefabData(const std::string& prefabName);

		//Returns scene data associated with sceneName
		HUGO::ErrorOrType<SceneData> getSceneData(const std::string& sceneName);
	};

}


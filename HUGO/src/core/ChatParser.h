#pragma once

#include <unordered_map>
#include <vector>
#include <HugoMath.h>
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

		//Open .chat file and read it
		HUGO::ErrorOrVoid readChat(const std::string& chatPath);

		//Read previously opened file as Scene.
		HUGO::ErrorOrVoid handleSceneType();

		//Reads Entity until it reads END_ENTITY
		HUGO::ErrorOrType<EntityData> handleEntity();

		//Read previously opened file as Prefab
		HUGO::ErrorOrVoid handlePrefabType();
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


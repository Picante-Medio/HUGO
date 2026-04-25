#pragma once

#include <unordered_map>
#include <vector>
#include <HugoMath.h>
#include "HugoError.h"
#include "HugoParsingVariant.h"


namespace HUGO{
	struct ComponentData {
		HUGO::String componentName;
		std::unordered_map<HUGO::String, HVariant> parameters;
	};

	using EntityData = std::vector<ComponentData>;

	struct SceneData {
		
		std::vector<EntityData> entities;
		std::vector<HUGO::String> systems;
	};

	class ChatParser {
	private:
		
		//Data that can be queried
		std::unordered_map<HUGO::String, SceneData> m_sceneData;
		std::unordered_map<HUGO::String, EntityData> m_prefabData;

		//Open .chat file and read it
		ErrorOrVoid readChat(const HUGO::String& chatPath);

		//Read previously opened file as Scene.
		ErrorOrVoid handleSceneType();
		//Read previously opened file as Prefab
		ErrorOrVoid handlePrefabType();
	public:

		//Initialize empty database. Function readChatFolder must be called to fill the database.
		ChatParser();

		//Adds to the database the data from all .chat files of chatFolder path.
		//@param chatFolder Path relative to the executable file (normally HUGO/bin)
		ErrorOrVoid readChatFolder(const HUGO::String& chatFolder);

		//Returns entity data associated with prefabName
		ErrorOrType<EntityData> getPrefabData(const HUGO::String& prefabName);

		//Returns scene data associated with sceneName
		ErrorOrType<SceneData> getSceneData(const HUGO::String& sceneName);
	};

}


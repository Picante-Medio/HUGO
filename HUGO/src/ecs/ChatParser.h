#pragma once

#include "Scene.h"
#include "Ecs.h"
#include <any>
#include <vector>
#include <memory>


namespace HUGO{
	struct ComponentData {
		HUGO::String componentName;
		std::vector<std::any> parameters;
	};

	using EntityData = std::vector<ComponentData>;

	struct SceneData {
		
		std::vector<EntityData> entities;
		std::vector<HUGO::String> systems;
	};

	//This class normally will be called by the ResourceManager in order to:
	// -Load in memory all the game´s scene and prefab information at once
	// -Query specific information in order to generate a scene or instantiate a prefab (This is done by the ResourceManager)
	class ChatParser {
	private:
		
		//Data that can be queried
		std::unordered_map<HUGO::String, SceneData> m_scene_data;
		std::unordered_map<HUGO::String, EntityData> m_prefab_data;

		void readChat(const HUGO::String& chatPath);
	public:

		//Initialize empty database. Function readChatFolder must be called to fill the database.
		ChatParser();

		//Adds to the database the data from all .chat files of chatFolder path
		void readChatFolder(const HUGO::String& chatFolder);

		//Returns entity data associated with prefabName
		EntityData getPrefabData(const HUGO::String& prefabName);

		//Returns scene data associated with sceneName
		SceneData getSceneData(const HUGO::String& sceneName);
	};

}


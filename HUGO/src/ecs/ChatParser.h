#pragma once

#include "Scene.h"
#include "Ecs.h"
#include <any>
#include <vector>
#include <memory>


namespace HUGO{
	//Forward declaration
	class ResourceManager;


	struct SceneData {
		struct ComponentData {
			std::string componentName;
			std::vector<std::any> parameters;
		};

		std::vector<std::vector<ComponentData>> entities;
		std::vector<std::string> systems;
	};

	class ChatParser {
	private:
		std::vector<SceneData> m_scenes;
		std::vector<SceneData::ComponentData> m_prefabs;

	public:

		ChatParser(std::shared_ptr<HUGO::ResourceManager>);

		void readChat(std::string& chatPath);

		void loadScene(HUGO::SceneID sceneID);
		void loadComponent(Scene& scene, std::string& componentName, EntityID entity, std::vector<std::any> args);
	};

}


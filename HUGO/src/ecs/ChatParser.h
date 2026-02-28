#include "Scene.h"
#include "Ecs.h"
#include <any>
#include <vector>

struct SceneData {
	struct ComponentData {
		std::string componentName;
		std::vector<std::any> parameters;
	}

	std::vector<std::vector<ComponentData>> entities;
	std::vector<std::string> systems;
}

class ChatParser {
private:
	std::vector<SceneData> scenes;
public:
	void ReadChat(std::string& chatPath);

	void LoadScene(HUGO::SceneID sceneID);
	void LoadComponent(Scene& scene, std::string& componentName, EntityID entity, std::vector<std::any> args);
};

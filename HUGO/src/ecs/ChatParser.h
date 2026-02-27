#include "Scene.h"
#include "Ecs.h"
#include <any>

class ChatParser {
	public:

	void ReadComponent(Scene& scene, std::string& componentName, EntityID entity, std::vector<std::any> args);
};

#include "Scene.h"
#include "Ecs.h"
#include <any>
#include "Audio3DInfoComponet.h"
#include "AudioComponent.h"
#include "RenderComponent.h"
#include "ViewportComponent.h"

class ChatParser {
	public:
		void ReadComponent(Scene& scene, std::string& componentName, EntityID entity, std::vector<std::any> args) {
			if("Audio3DInfoComponet" == componentName) 
				scene.addComponent<Audio3DInfoComponet>(entity, args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9]);
			else if("AudioComponent" == componentName) 
				scene.addComponent<AudioComponent>(entity, args[0],args[1],args[2],args[3]);
			else if("RenderComponent" == componentName) 
				scene.addComponent<RenderComponent>(entity, args[0],args[1],args[2]);
			else if("ViewportComponent" == componentName) 
				scene.addComponent<ViewportComponent>(entity, args[0],args[1],args[2],args[3],args[4],args[5]);
		}
};

#include "Scene.h"
#include "Audio3DInfoComponet.h"
#include "AudioComponent.h"
#include "RenderComponent.h"
#include "ViewportComponent.h"

class SceneParser {
	public:
		void ReadComponent(Scene& scene, string& componentName, EnitityID entity, std::vector<std::any> args) {
			if("Audio3DInfoComponet" == componentName) scene.addComponent<Audio3DInfoComponet>(entity, args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7],args[8],args[9]);
			else("AudioComponent" == componentName) scene.addComponent<AudioComponent>(entity, args[0],args[1],args[2],args[3]);
			else("RenderComponent" == componentName) scene.addComponent<RenderComponent>(entity, args[0],args[1],args[2]);
			else("ViewportComponent" == componentName) scene.addComponent<ViewportComponent>(entity, args[0],args[1],args[2],args[3],args[4],args[5]);
		}
}

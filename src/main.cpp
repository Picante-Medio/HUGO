#include "SDL.h"
#include "Ogre.h"
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

#include "ogre/InitOgre.h"
#include "utils/HugoVirtualTimer.h"
#include "ecs/Scene.h"
#include "systems/render/RenderSystem.h"


int main(int argc, char* argv[]) {

    // std::shared_ptr<Ogre::Root> mRoot = HUGO::initializeOgre();
    // auto scenePtr = std::make_shared<Scene>();
    // RenderSystem renderSystem(scenePtr, mRoot);
    // scenePtr->addSystem(renderSystem);

    // HUGO::VirtualTimer vt;
    // vt.reset();

    // bool exit = false;
//	while (!exit) {
        // vt.update();
		// double starTime = vt.getDeltaTime();
		
        // scenePtr->fixedUpdate(starTime);    
        // scenePtr->update(starTime);

        // mRoot->renderOneFrame();
		// double frameTime = vt.getDeltaTime() - starTime;
		// if (frameTime < 10) SDL_Delay(10- frameTime);
//	}

    return 0;
}

#include "SDL.h"
#include "Ogre.h"
#include <OgreOverlaySystem.h>
#include <OgreOverlayManager.h>
#include <OgreTextAreaOverlayElement.h>

#include "core/Core.h"
#include "utils/HugoVirtualTimer.h"
#include "ecs/Scene.h"
#include "systems/render/RenderSystem.h"


// prueba para ver que reconoce miniaudio
#include "miniaudio.h"

#include <iostream>

int main(int argc, char* argv[]) {

    // std::pair<std::shared_ptr<Ogre::Root>, Ogre::RenderWindow*> ogreIni = HUGO::initializeOgre();
    // auto scenePtr = std::make_shared<Scene>();
    // RenderSystem renderSystem(scenePtr, ogreIni.first, ogreIni.second);
    // scenePtr->addSystem(renderSystem);
    // renderSystem.setfullScreen();

    // HUGO::VirtualTimer vt;
    // vt.reset();

    // bool exit = false;
	// while (!exit) {
    //     vt.update();
	// 	double starTime = vt.getDeltaTime();

    //     scenePtr->fixedUpdate(starTime);
    //     scenePtr->update(starTime);

    //     ogreIni.first->renderOneFrame();
	// 	double frameTime = vt.getDeltaTime() - starTime;
	// 	if (frameTime < 10) SDL_Delay(10- frameTime);
	// }

    HUGO::Core();

    std::cout << "Prueba de funcionamiento\n";

    return 0;
}

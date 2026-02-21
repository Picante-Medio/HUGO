#include "Core.h"
#include "Ogre.h"
#include "RenderSystem.h"
#include "Scene.h"
#include <SDL2/SDL.h>
#include <iostream>

using namespace HUGO;


Core::Core(){
    
}

//Inicialización de los módulos de HUGO y carga de la .dll del juego
void Core::init(){
    vt.reset();
    initOgre();

    // scenePtr = std::make_shared<Scene>();
    // RenderSystem renderSystem(scenePtr, m_ogreRoot, m_renderWindow);
    // scenePtr->addSystem(renderSystem);
    // renderSystem.setfullScreen();

}

//Destrucción de los módulos del motor. Final de HUGO
void Core::destroy(){
    destroyOgre();
}

//--------------------------- INICIALIZACIÓN MÓDULOS --------------------

void Core::initOgre(){
    m_ogreRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");

    if (!m_ogreRoot->restoreConfig()) {
        Ogre::LogManager::getSingleton().logMessage("Initialize::configure_ogre => " +
        std::string("ERROR: unable to configure Ogre"));
    }
    for(Ogre::RenderSystem* renderSys: m_ogreRoot->getAvailableRenderers()){
        if(renderSys->getName() == "OpenGL Rendering Subsystem"){
            m_ogreRoot->setRenderSystem(renderSys);
            break;
        }
    }

    m_ogreRoot->getRenderSystem()->setConfigOption("Full Screen", "No");

    m_ogreRoot->initialise(false);

    // creation of the render window, with the size 1000x800 and not full screen
    Ogre::NameValuePairList params;
    m_renderWindow = m_ogreRoot->createRenderWindow(
        "Ogre Test Window",
        1000, // width
        800,  // height
        false,
        &params
    );
}

//Bucle principal de HUGO
void Core::run(){
    // bool exit = false;
	// while (!exit) {
    //     vt.update();
	// 	double starTime = vt.getDeltaTime();

    //     scenePtr->fixedUpdate(starTime);
    //     scenePtr->update(starTime);

    //     m_ogreRoot->renderOneFrame();
	// 	double frameTime = vt.getDeltaTime() - starTime;
	// 	if (frameTime < 10) SDL_Delay(10- frameTime);
	// }
}


void Core::destroyOgre(){

}
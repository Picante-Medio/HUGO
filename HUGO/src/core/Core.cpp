#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>

#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <cstddef>
#include <iostream>
#include <Ogre.h>
#include <OgreOverlaySystem.h>
#include <memory>
#include <SDL2/SDL_syswm.h>
#include <string>

#include "HugoError.h"
#include "Input.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "UISystem.h"
#include "Scene.h"
#include "BulletModule.h"
//#include "ResourceManager.h"

#include "ChatParser.h"
#include "ComponentRegistryInitializer.h"

#include "LuaBinder.h"

#include "GameLibraryLoader.h"

// se podría hacer clase singleton¿? -Marco
using namespace HUGO;
Core::Core(): sceneManager()
{

}

//Initialization of HUGO modules and loading of games dynamic lib
void Core::init(){

    // Inicializar registro de componentes
    ComponentRegistry::setInstance(std::make_shared<ComponentRegistryInitializer>());

    // Carga del juego como biblioteca dinamica
    GameLibraryLoader::init("TestGame");

    //LuaBinder::Instance().Init();

    vt.reset();
    initModules();

    ErrorOrVoid err = sceneManager.initializeParser("../chats", physicsModule);
    
    sceneManager.setCurrentScene("TestScene");
    
    /*scenePtr = std::make_shared<Scene>();

    ChatParser chatParser = ChatParser();
    ErrorOrVoid err = chatParser.readChatFolder("../chats");

    std::shared_ptr<AudioSystem> audioSystem = std::make_shared<AudioSystem>(scenePtr);
    audioSystem->init();

    //audioSystem->loadAudiosFromFile("AudioList.txt");
    scenePtr->addSystem(audioSystem);
    
    std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(scenePtr);
    renderSystem->init();
    scenePtr->addSystem(renderSystem);

    std::shared_ptr<UISystem> uiSystem = std::make_shared<UISystem>(scenePtr);
    uiSystem->init();
    scenePtr->addSystem(uiSystem);
    //uiSystem->createTestCanvas();

    
    //m_resourceManager = std::make_shared<HUGO::ResourceManager>(maEngine, m_ogreRoot);

    // Physics testing
    physicsModule->createSceneSimulation(scenePtr);
    std::shared_ptr<PhysicsTestSystem> testSystem = std::make_shared<PhysicsTestSystem>(scenePtr);
    testSystem->init();
    scenePtr->addSystem(testSystem);
    */


}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    //MiniaudioModule init
    MiniaudioModule::Instance();

    //OgreModule init
    OgreModule::Instance();

    //BulletoModule init
    physicsModule = std::make_shared<BulletModule>();

    //LuaBinder init
    LuaBinder& luaBinder = LuaBinder::Instance();
    luaBinder.Init();
}
//HUGO main loop
void Core::run(){
  bool exit = false;
  Input& input = Input::Instance();
  
	while (!exit) {
        vt.update();
    
        double startTime = vt.getDeltaTime()/1000.0; //ms->s

        input.handleInput();
        if(input.isQuitPressed()) {
            exit = true;
        }
        sceneManager.fixedUpdateCurrentScene(startTime);
        physicsModule->performSimulations(startTime);
        sceneManager.updateCurrentScene(startTime);

        physicsModule->debugDraw();
        
        //m_ogreModule->renderOneFrame();
        OgreModule::Instance().renderOneFrame();

        double frameTime = vt.getDeltaTime() - startTime;
        if (frameTime < FRAME_RATE) {
            SDL_Delay(static_cast<Uint32>(FRAME_RATE-frameTime));
        }
	}
}

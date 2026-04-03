#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>

#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <cstddef>
#include <filesystem>
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
Core::Core(): input(std::make_shared<Input>()), sceneManager(input)
{

}

//Initialization of HUGO modules and loading of games dynamic lib
ErrorOrVoid Core::init(){

    // Inicializar registro de componentes
    ComponentRegistry::setInstance(std::make_shared<ComponentRegistryInitializer>());

    // Carga del juego como biblioteca dinamica
    GameLibraryLoader::init("TestGame");

    vt.reset();
    initModules();

    ErrorOrVoid err = sceneManager.initializeParser("../chats", physicsModule);

    if(err.isError()) return err;
    

    /*
        Should be changed when implemented in GODOT to:
        
        err = sceneManager.setMainScene();
        if(err.isError()) return err;
    */
    sceneManager.setCurrentScene("TestScene");

    return ErrorOrVoid();
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

    //LuaBinder initialization
    initLua();

    // Input
    input->setSDLWindow(OgreModule::Instance().getSDLWindow());
}
//HUGO main loop
void Core::run(){
  bool exit = false;

	while (!exit) {
        vt.update();

        double startTime = vt.getDeltaTime()/1000.0; //ms->s

        input->handleInput();
        if(input->isQuitPressed()) {
            exit = true;
        }
        sceneManager.fixedUpdateCurrentScene(startTime);
        physicsModule->performSimulations(startTime);
        sceneManager.updateCurrentScene(startTime);
        sceneManager.dispatchMessagesCurrentScene();

        //physicsModule->debugDraw();
        
        //m_ogreModule->renderOneFrame();
        OgreModule::Instance().renderOneFrame();

        double frameTime = vt.getDeltaTime() - startTime;
        if (frameTime < FRAME_RATE) {
            SDL_Delay(static_cast<Uint32>(FRAME_RATE-frameTime));
        }
	}
}


void 
Core::initLua() {
    //LuaBinder init
    std::shared_ptr<LuaBinder> instance = std::make_shared<LuaBinder>();
    instance->Init();
    LuaBinder::setInstance(instance);

    std::filesystem::path luaPath = std::filesystem::absolute(
        std::filesystem::path("..") / "src" / "lua" / "scripts"
    );
    LuaBinder::getInstance()->requireFolder(luaPath.string());
}

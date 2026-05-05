#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>

#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <filesystem>
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
#include "InternalScene.h"
#include "BulletModule.h"
//#include "ResourceManager.h"

#include "ChatParser.h"

#include "LuaBinder.h"

#include "GameLibraryLoader.h"

// se podría hacer clase singleton¿? -Marco
using namespace HUGO;
Core::Core(): input(std::make_shared<Input>()), sceneManager(input)
{

}

//Initialization of HUGO modules and loading of games dynamic lib
HUGO::ErrorOrVoid Core::init(int argc, char* argv[]){
    std::string gamePath = "TestGame";
    std::string chatPath = "../chats";
    std::string mainSceneName = "TestSceneVideo1";

    if (argc > 1) gamePath = argv[1];
    if (argc > 2) chatPath = argv[2];
    if (argc > 3) mainSceneName = argv[3];

    //LuaBinder initialization
    initLua();

    // Load game as dll
    GameLibraryLoader::init(gamePath);

    vt.reset();
    initModules();

    HUGO::ErrorOrVoid err = sceneManager.initializeParser(chatPath, physicsModule);

    if(err.isError()) return err;
    
    sceneManager.setCurrentScene(mainSceneName);

    return HUGO::ErrorOrVoid();
}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    //MiniaudioModule init
    MiniaudioModule::Instance();
    //Audio assets init
    initAudioAssets();

    //OgreModule init
    OgreModule::Instance();

    //BulletoModule init
    physicsModule = std::make_shared<BulletModule>();


    // Input
    input->setSDLWindow(OgreModule::Instance().getSDLWindow());
}
//HUGO main loop
HUGO::ErrorOrVoid Core::run(){
    bool exit = false;
    //If an error is detected, this value is returned
    HUGO::ErrorOrVoid exitValue = HUGO::ErrorOrVoid();

    int32_t accumulator = 0;

	while (!exit) {
        vt.update();

        uint32_t frameTime = vt.getDeltaTime(); //ms
        accumulator += frameTime;

        exitValue = sceneManager.sceneChangeRequested();
        if(sceneManager.getQuit() || exitValue.isError()) exit = true;
        else{
            while(accumulator > FIXED_STEP) {
                sceneManager.fixedUpdateCurrentScene(FIXED_STEP);
                physicsModule->performSimulations(FIXED_STEP);
                accumulator -= FIXED_STEP;
            }
    
            input->handleInput();
            if(input->isQuitPressed()) {
                exit = true;
            }
            if(input->windowHasChanged()){   
                OgreModule::Instance().updateWindowDimensions();
                input->clearWindowFlag();
            }
            sceneManager.updateCurrentScene(frameTime);
            sceneManager.dispatchMessagesCurrentScene();
            
            OgreModule::Instance().renderOneFrame();

        }

	}

    return exitValue;
}


void 
Core::initLua() {
    //LuaBinder init

    const std::filesystem::path luaPath = std::filesystem::absolute(
        std::filesystem::path("..") / "src" / "lua"
    );

    if(std::filesystem::exists(luaPath))
        LuaBinder::getInstance().requireFolder(luaPath.string());
    
}

void HUGO::Core::initAudioAssets()
{
    auto& assets = AudioAssetsModule::Instance();
    
    // set base path
    assets.setBasePath("../Resources/audios/");
    
    // automatically scan and register all audio files
    assets.scanAndRegisterAll();
}

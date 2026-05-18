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
#include <unordered_map>
#include <iostream>
#include <fstream>

#include "HugoError.h"
#include "Input.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "UISystem.h"
#include "InternalScene.h"
#include "BulletModule.h"

#include "ChatParser.h"

#include "LuaBinder.h"

#include "GameLibraryLoader.h"

using namespace HUGO;
Core::Core(): input(std::make_shared<Input>()), sceneManager(input)
{}

//Initialization of HUGO modules and loading of games dynamic lib
HUGO::ErrorOrVoid Core::init() {

    std::unordered_map<std::string, std::string> initParams = std::unordered_map<std::string, std::string>();

    // Read config file

    std::ifstream configFile("./hugo.config");

    if (!configFile.is_open()) {
        
        throw "hugo.config not found";
    }

    std::string line;

    while (std::getline(configFile, line)) {
        // Ignore empty lines and commented (start with #)
        if(line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');

        if(pos == std::string::npos) {
            std::cerr << "[hugo.config] Invalid line: " << line;
            continue;
        } 

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        if((key == "GAME_LIB" || key == "CHATS" || key == "MAIN_SCENE" || key == "FULLSCREEN")
        && value.size() > 0) {
            if (initParams.count(key)) {
                std::cerr << "[hugo.config] " << key << " field duplcated at line: " << line;
                continue;
            }

            initParams[key] = value;
        }
    }

    configFile.close();

    // Validation part

    // Default chat location
    if(!initParams.count("CHATS")) {
        initParams["CHATS"] = "../chats";
    }
    // Default fullscreen is false
    if(!initParams.count("FULLSCREEN") || (initParams["FULLSCREEN"] != "TRUE")) {
        fullscreen = false;
    }
    else {
        fullscreen = true;
    }

    if(!initParams.count("GAME_LIB") || !initParams.count("MAIN_SCENE")) {
        throw "[hugo.config] GAME_LIB or MAIN_SCENE field empty or missing";
    }

    gameName = initParams["GAME_LIB"].c_str();

    //LuaBinder initialization
    initLua();

    // Load game as dll
    GameLibraryLoader::init(initParams["GAME_LIB"]);

    vt.reset();
    initModules();

    HUGO::ErrorOrVoid err = sceneManager.initializeParser(initParams["CHATS"], physicsModule);

    if(err.isError()) return err;
    
    sceneManager.setCurrentScene(initParams["MAIN_SCENE"]);

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
    OgreModule::Instance().setFullscreen(fullscreen);

    //BulletoModule init
    physicsModule = std::make_shared<BulletModule>();

    // Input
    input->setSDLWindow(OgreModule::Instance().getSDLWindow());

    SDL_SetWindowTitle(OgreModule::Instance().getSDLWindow(), gameName);
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

        exitValue = sceneManager.manageQueries();
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

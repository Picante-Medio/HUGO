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

#include "GameLibrary.h"

using namespace HUGO;
Core::Core() {
    processHugoConfig();
    initializeDependencies();
    loadGameLibrary();
    loadSceneManager();
}

Core::~Core() {
    destroySceneManager();
    unloadGameLibrary();
    destroyDependencies();
}

void Core::processHugoConfig() {

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
                std::cerr << "[hugo.config] " << key << " field duplcated at line: " << line << "\n";
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

    gameName = initParams["GAME_LIB"];
    chatsDirectory = initParams["CHATS"];
    initialScene = initParams["MAIN_SCENE"];
}

void Core::initializeDependencies() {

    //LuaBinder initialization
    initLua();

    //MiniaudioModule init
    MiniaudioModule::instance();

    // set base path
    AudioAssetsModule::instance().setBasePath("../Resources/audios/");
    // automatically scan and register all audio files
    AudioAssetsModule::instance().scanAndRegisterAll();

    //OgreModule init
    OgreModule::instance();
    OgreModule::instance().setFullscreen(fullscreen);

    //BulletoModule init
    physicsModule = std::make_unique<BulletModule>();

    // Input
    input = std::make_unique<Input>();
    input->setSDLWindow(OgreModule::instance().getSDLWindow());
    SDL_SetWindowTitle(OgreModule::instance().getSDLWindow(), gameName.c_str());
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

void Core::loadGameLibrary() {
    gameInstance = std::make_unique<GameLibrary>(gameName);
}

void Core::loadSceneManager() {
    sceneManager = std::make_unique<SceneManager>(*input, chatsDirectory, *physicsModule);
    sceneManager->setCurrentScene(initialScene);
}

void Core::destroySceneManager() {
    sceneManager.reset();
}

void Core::unloadGameLibrary() {
    gameInstance.reset();
}

void Core::destroyDependencies() {

    input.reset();
    OgreModule::destroyInstance();
    AudioAssetsModule::destroyInstance();
    MiniaudioModule::destroyInstance();
    // Lua is not necesary to destroy
}

//HUGO main loop
HUGO::ErrorOrVoid Core::run(){

    vt.reset();

    bool exit = false;
    //If an error is detected, this value is returned
    HUGO::ErrorOrVoid exitValue = HUGO::ErrorOrVoid();

    int32_t accumulator = 0;

	while (!exit) {
        vt.update();

        uint32_t frameTime = vt.getDeltaTime(); //ms
        accumulator += frameTime;

        exitValue = sceneManager->manageQueries();
        if(sceneManager->getQuit() || exitValue.isError()) exit = true;
        else{
            while(accumulator > FIXED_STEP) {
                sceneManager->fixedUpdateCurrentScene(FIXED_STEP);
                physicsModule->performSimulations(FIXED_STEP);
                accumulator -= FIXED_STEP;
            }
    
            input->handleInput();
            if(input->isQuitPressed()) {
                exit = true;
            }
            if(input->windowHasChanged()){   
                OgreModule::instance().updateWindowDimensions();
                input->clearWindowFlag();
            }
            sceneManager->updateCurrentScene(frameTime);
            sceneManager->dispatchMessagesCurrentScene();
            
            OgreModule::instance().renderOneFrame();

        }

	}

    return exitValue;
}


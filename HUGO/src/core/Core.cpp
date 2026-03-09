#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>


#include <cstddef>
#include <iostream>
#include <Ogre.h>
#include <memory>
#include <string>

#include "HugoError.h"
#include "Input.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "UISystem.h"
#include "Scene.h"
#include "BulletModule.h"
//#include "ResourceManager.h"

#include "PhysicsTestSystem.h"

#include "ChatParser.h"
#include "ComponentRegistryInitializer.h"

#include "LuaBinder.h"

// se podría hacer clase singleton¿? -Marco
using namespace HUGO;
Core::Core()
{
}

//Initialization of HUGO modules and loading of games dynamic lib
void Core::init(){

    //LuaBinder::Instance().Init();

    ComponentRegistry::setInstance(std::make_shared<ComponentRegistryInitializer>());

    vt.reset();
    initModules();
    
    scenePtr = std::make_shared<Scene>();

    ChatParser chatParser = ChatParser();
    ErrorOrVoid err = chatParser.readChatFolder("../chats");

    std::shared_ptr<AudioSystem> audioSystem = std::make_shared<AudioSystem>(scenePtr, maEngine);
    audioSystem->loadAudiosFromFile("AudioList.txt");
    scenePtr->addSystem(audioSystem);
    audioSystem->createTestEntity();
    
    std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(scenePtr);
    scenePtr->addSystem(renderSystem);

    std::shared_ptr<UISystem> uiSystem = std::make_shared<UISystem>(scenePtr);
    scenePtr->addSystem(uiSystem);

    
    //m_resourceManager = std::make_shared<HUGO::ResourceManager>(maEngine, m_ogreRoot);

    // Physics testing
    physicsModule->createSceneSimulation(scenePtr);
    std::shared_ptr<PhysicsTestSystem> testSystem = std::make_shared<PhysicsTestSystem>(scenePtr);
    scenePtr->addSystem(testSystem);
}

//Destruction of HUGO modules. End of HUGO
void Core::destroy(){
    destroyMiniaudio();
    destroyPhysicsModule();
}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    initMiniaudio();
    initPhysicsModule();
    initLua();
}

//HUGO main loop
void Core::run(){
  bool exit = false;
  Input& input = Input::Instance();
  
	while (!exit) {
        vt.update();
    
        double startTime = vt.getDeltaTime();

        input.handleInput();
        if(input.isQuitPressed()) {
            exit = true;
        }
        scenePtr->fixedUpdate(startTime);
        physicsModule->performSimulations(startTime);
        scenePtr->update(startTime);

        double frameTime = vt.getDeltaTime() - startTime;

        if (frameTime < 0.016) {
            SDL_Delay(static_cast<Uint32>((0.016 - frameTime) * 1000.0));
        }
	}
}

void HUGO::Core::initMiniaudio()
{
    maEngine = std::make_shared<ma_engine>();
    ma_result result = ma_engine_init(nullptr, maEngine.get());
    if (result != MA_SUCCESS) {
        throw std::runtime_error("Failed to initialize ma_engine");
    }
    else std::cout << "ma_engine initialized correctly\n";
}

void HUGO::Core::destroyMiniaudio()
{
    ma_engine_uninit(maEngine.get());
}

void HUGO::Core::initPhysicsModule() {
    physicsModule = std::make_shared<BulletModule>();
}

void HUGO::Core::destroyPhysicsModule() {
    // ...
}

void HUGO::Core::initLua() {
    LuaBinder& luaBinder = LuaBinder::Instance();
    luaBinder.Init();
}

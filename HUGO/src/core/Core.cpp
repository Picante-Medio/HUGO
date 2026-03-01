#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>

#include <SDL2/SDL.h>
#include <SDL_video.h>
#include <cstddef>
#include <iostream>
#include <Ogre.h>
#include <memory>
#include <SDL2/SDL_syswm.h>

#include "Input.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "PhysicsSystem.h"
#include "Scene.h"
//#include "ResourceManager.h"

// se podría hacer clase singleton¿? -Marco
using namespace HUGO;
Core::Core()
{
}

//Initialization of HUGO modules and loading of games dynamic lib
void Core::init(){
    vt.reset();
    initModules();
    
    scenePtr = std::make_shared<Scene>();

    std::shared_ptr<AudioSystem> audioSystem = std::make_shared<AudioSystem>(scenePtr, maEngine);
    scenePtr->addSystem(audioSystem);
    //audioSystem->createTestEntity();
    
    std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(scenePtr, m_ogreRoot, m_renderWindow);
    scenePtr->addSystem(renderSystem);

    //std::shared_ptr<PhysicsSystem> physicsSystem = std::make_shared<PhysicsSystem>(scenePtr);
    
    //m_resourceManager = std::make_shared<HUGO::ResourceManager>(maEngine, m_ogreRoot);
    renderSystem->createTestEntity();
}

//Destruction of HUGO modules. End of HUGO
void Core::destroy(){
    destroyOgre();
    destroyMiniaudio();
}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    initMiniaudio();
    initOgre();
}

void Core::initOgre()
{
    m_ogreRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");
    if (!m_ogreRoot->restoreConfig()) {
        Ogre::LogManager::getSingleton().logMessage("Initialize::configure_ogre => " +
        std::string("ERROR: unable to configure Ogre"));
    }
    //If there is another rendering system available, we could choose between one or another
    for(Ogre::RenderSystem* renderSys: m_ogreRoot->getAvailableRenderers()){
        if(renderSys->getName() == "OpenGL Rendering Subsystem"){
            m_ogreRoot->setRenderSystem(renderSys);
            break;
        }
    }

    m_ogreRoot->getRenderSystem()->setConfigOption("Full Screen", "No");

    m_ogreRoot->initialise();
    locateResources();
    loadResources();

    Ogre::NameValuePairList params;
    params["SDL2Window"] = "true";
    m_renderWindow = m_ogreRoot->createRenderWindow("Ogre Test Window", 1000, 800, false, &params);

    unsigned long windowHandle = 0;
    m_renderWindow->getCustomAttribute("WINDOW", &windowHandle);
    SDL_Log("EGL window handle: %lu", windowHandle);

    SDL_Init(SDL_INIT_EVENTS);
    SDL_CreateWindowFrom((void*)windowHandle);
}

void HUGO::Core::locateResources()
{
  Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

  Ogre::ConfigFile::SettingsBySection_ seci = cf.getSettingsBySection();
    Ogre::String sec, type, arch;
    for(auto& s : cf.getSettingsBySection()) {
        sec = s.first;
        for (auto& t : s.second)
        {
            type = t.first;
            arch = t.second;
            rgm.addResourceLocation(arch,type,sec);
        }
    }
}
void HUGO::Core::loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//HUGO main loop
void Core::run(){
    bool exit = false;

	while (!exit) {
        vt.update();
		double startTime = vt.getDeltaTime();
        Input::Instance().handleInput();
        scenePtr->fixedUpdate(startTime);
        scenePtr->stepPhysics(startTime);
        scenePtr->update(startTime);

        m_ogreRoot->renderOneFrame();
		double frameTime = vt.getDeltaTime() - startTime;
		if (frameTime < 10) SDL_Delay(10- frameTime);
	}
}


void Core::destroyOgre(){
    m_renderWindow->destroy();

    m_ogreRoot->shutdown();

    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
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

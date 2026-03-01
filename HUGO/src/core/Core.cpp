#include "Core.h"

#include <OgreCommon.h>
#include <OgreConfigFile.h>
#include <OgreResourceGroupManager.h>
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
    locateResources();
    m_ogreRoot->initialise(false); //To ensure ogre knows sdl is the window owner

#ifdef __linux__
    SDL_setenv("SDL_VIDEODRIVER", "x11", 1);
#endif
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_Window* sdlWindow = SDL_CreateWindow("Ogre", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 800, SDL_WINDOW_OPENGL);
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(sdlWindow, &wmInfo);
    SDL_Log("Video driver: %s", SDL_GetCurrentVideoDriver());
    loadResources();
    
    Ogre::NameValuePairList params;
#ifdef __linux__
    size_t windowHandle = reinterpret_cast<size_t>(wmInfo.info.x11.window);
#elif _WIN32
    size_t windowHandle = reinterpret_cast<size_t>(wmInfo.info.win.window);
#endif
    params["externalWindowHandle"] = Ogre::StringConverter::toString(windowHandle);
    m_renderWindow = m_ogreRoot->createRenderWindow("Ogre Test Window", 1000, 800, false, &params);
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

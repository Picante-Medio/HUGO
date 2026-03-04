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
#include "Scene.h"
#include "BulletModule.h"
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
    audioSystem->loadAudiosFromFile("test_media/AudioList.txt");
    scenePtr->addSystem(audioSystem);
    audioSystem->createTestEntity();
    
    std::shared_ptr<RenderSystem> renderSystem = std::make_shared<RenderSystem>(scenePtr, m_ogreRoot, m_renderWindow);
    scenePtr->addSystem(renderSystem);
    renderSystem->createTestEntity();
    
    //m_resourceManager = std::make_shared<HUGO::ResourceManager>(maEngine, m_ogreRoot);
}

//Destruction of HUGO modules. End of HUGO
void Core::destroy(){
    destroyOgre();
    destroyMiniaudio();
    destroyPhysicsModule();
}

//--------------------------- IMODULES INITIALIZATION --------------------

void HUGO::Core::initModules()
{
    initMiniaudio();
    initOgre();
    initPhysicsModule();
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

    m_ogreRoot->initialise(false);
    locateResources();
    loadResources();

    #ifdef __linux__
        SDL_setenv("SDL_VIDEODRIVER", "x11", 1);
    #endif

    if (SDL_Init(SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error("SDL_Init failed");
    }
    
    sdlWindow = SDL_CreateWindow("Ogre Test Window",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    1000, 800, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(sdlWindow, &wmInfo)) {
        throw std::runtime_error("SDL_GetWindowWMInfo failed");
    }
    SDL_Log("Video driver: %s", SDL_GetCurrentVideoDriver());

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

        m_ogreRoot->renderOneFrame();

        double frameTime = vt.getDeltaTime() - startTime;

        if (frameTime < 0.016) {
            SDL_Delay(static_cast<Uint32>((0.016 - frameTime) * 1000.0));
        }
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

void HUGO::Core::initPhysicsModule() {
    physicsModule = std::make_shared<BulletModule>();
}

void HUGO::Core::destroyPhysicsModule() {
    // ...
}
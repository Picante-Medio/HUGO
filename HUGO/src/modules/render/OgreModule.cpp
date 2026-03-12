
#include "OgreModule.h"

#include <Ogre.h>
#include <OgreRoot.h>
#include "OgreRenderWindow.h"
#include "OgreOverlaySystem.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#pragma region Resource Helper Functions

void locateResources()
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
void loadResources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

#pragma endregion

OgreModule::OgreModule(){
        m_ogreRoot = std::make_shared<Ogre::Root>("plugins.cfg", "ogre.cfg");
    if (!m_ogreRoot->restoreConfig()) {
        Ogre::LogManager::getSingleton().logMessage("Initialize::configure_ogre => " +
        std::string("ERROR: unable to configure Ogre"));
    }
    //If there is another rendering system available, we could choose between one or another
    for(Ogre::RenderSystem* renderSys : m_ogreRoot->getAvailableRenderers()){
        if(renderSys->getName() == "OpenGL Rendering Subsystem"){
            m_ogreRoot->setRenderSystem(renderSys);
            break;
        }
    }

    m_ogreRoot->getRenderSystem()->setConfigOption("Full Screen", "No");

    //Must be done before initializing Ogre::Root
    m_ogreOverlaySystem = std::make_shared<Ogre::OverlaySystem>();

    m_ogreRoot->initialise(false);

    locateResources();
    loadResources();

    #ifdef __linux__
        SDL_setenv("SDL_VIDEODRIVER", "x11", 1);
    #endif

    if (SDL_Init(SDL_INIT_EVENTS) != 0) {
        throw std::runtime_error("SDL_Init failed");
    }
    
    m_sdlWindow = SDL_CreateWindow("Ogre Test Window",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    1000, 800, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    if (!SDL_GetWindowWMInfo(m_sdlWindow, &wmInfo)) {
        throw std::runtime_error("SDL_GetWindowWMInfo failed");
    }
    SDL_Log("Video driver: %s", SDL_GetCurrentVideoDriver());
    //SDL_SetWindowFullscreen(sdlWindow,SDL_WINDOW_FULLSCREEN_DESKTOP);

    Ogre::NameValuePairList params;
    #ifdef __linux__
    size_t windowHandle = reinterpret_cast<size_t>(wmInfo.info.x11.window);
    #elif _WIN32
    size_t windowHandle = reinterpret_cast<size_t>(wmInfo.info.win.window);
    #endif
    
    params["externalWindowHandle"] = Ogre::StringConverter::toString(windowHandle);
    m_ogreRenderWindow = m_ogreRoot->createRenderWindow(OGRE_WINDOW_NAME, 1000, 800, false, &params);
}

OgreModule::~OgreModule(){

    m_ogreRenderWindow->destroy();

    m_ogreRoot->shutdown();

    SDL_DestroyWindow(m_sdlWindow);
    SDL_Quit();
}

Ogre::RenderWindow* OgreModule::getOgreRenderWindow(){
    return m_ogreRenderWindow;
}

void OgreModule::renderOneFrame(){
    m_ogreRoot->renderOneFrame();
}
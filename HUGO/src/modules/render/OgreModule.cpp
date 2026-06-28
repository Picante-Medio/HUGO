
#include "OgreModule.h"

#include <Ogre.h>
#include <OgreRoot.h>
#include <OgreOverlayManager.h>
#include "OgreRenderWindow.h"
#include "OgreOverlaySystem.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

#include <iostream>

using std::filesystem::directory_entry;
using std::filesystem::directory_iterator;


OgreModule* OgreModule::instance = nullptr;

OgreModule&
OgreModule::getInstance() {
    if(instance) 
    return *instance;
    else 
    throw "Tried to access OgreModule instance before initialization or after destruction";
}

void 
OgreModule::createInstance() {
    if(instance) 
    delete instance;
    
    instance = new OgreModule();
}

void
OgreModule::destroyInstance() {
    if (!instance) return;
    
    delete instance;
    instance = nullptr;
}

OgreModule::OgreModule(){
    m_ogreRoot = std::make_unique<Ogre::Root>("", "");
    
    loadPlugins();
    
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
    m_ogreOverlaySystem = std::make_unique<Ogre::OverlaySystem>();
    
    m_ogreRoot->initialise(false);
    
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
    
    //--------------- VIEWPORT -------------------
    m_defaultCameraSceneManager = m_ogreRoot->createSceneManager();
    m_defaultCamera = m_defaultCameraSceneManager->createCamera("Default_camera");
    m_viewport = m_ogreRenderWindow->addViewport(m_defaultCamera, 0, 0.f, 0.f, 1.f, 1.f); 
    // default BackgroundColor is black
    m_viewport->setBackgroundColour(Ogre::ColourValue(0.3f,0.3f,0.5f,1.f));
    m_viewport->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
    
    loadResources();
}

OgreModule::~OgreModule(){
    
    // Destroy scene managers
    for(auto& sm: m_sceneManagers){
        if(sm.second){
            sm.second->clearScene();
            m_ogreRoot->destroySceneManager(sm.second);
        }
    }
    m_sceneManagers.clear();
    
    // Destroy viewport/camera scene manager
    if(m_defaultCameraSceneManager){
        m_defaultCameraSceneManager->clearScene();
        m_ogreRoot->destroySceneManager(m_defaultCameraSceneManager);
        m_defaultCameraSceneManager = nullptr;
    }
    
    Ogre::TextureManager::getSingleton().removeAll();
    Ogre::MaterialManager::getSingleton().removeAll();
    Ogre::MeshManager::getSingleton().removeAll();
    Ogre::OverlayManager::getSingleton().destroyAll();
    
    m_ogreRenderWindow->destroy();
    
    m_ogreOverlaySystem.reset(nullptr);
    m_ogreRoot.reset(nullptr);
    
    SDL_DestroyWindow(m_sdlWindow);
    SDL_Quit();
}
        
void OgreModule::setCurrentSceneManager(std::string sceneName){
    
    //If requested sceneManager was not created, we create it
    if(m_sceneManagers.find(sceneName) == m_sceneManagers.end()){
        createSceneManager(sceneName);
    }
    
    m_currentSceneManager = m_sceneManagers[sceneName];
}

Ogre::SceneManager* OgreModule::getCurrentSceneManager(){
    return m_currentSceneManager;
}

Ogre::Viewport* OgreModule::getViewport(){
    return m_viewport;
}

void OgreModule::viewportToDefaultCamera(){
    m_viewport->setCamera(m_defaultCamera);
}

void OgreModule::createSceneManager(std::string sceneName){
    if(m_sceneManagers.find(sceneName) == m_sceneManagers.end())
    m_sceneManagers[sceneName] = m_ogreRoot->createSceneManager();
}

void OgreModule::destroySceneManager(std::string sceneName){
    if (!m_sceneManagers.count(sceneName)) return;
    
    if (m_sceneManagers[sceneName] == m_currentSceneManager) m_currentSceneManager = nullptr;
    m_sceneManagers[sceneName]->clearScene();
    m_ogreRoot->destroySceneManager(m_sceneManagers[sceneName]);
    
    m_sceneManagers.erase(sceneName);
}

Ogre::RenderWindow* OgreModule::getOgreRenderWindow() const
{
    return m_ogreRenderWindow;
}

SDL_Window *OgreModule::getSDLWindow() const
{
    return m_sdlWindow;
}

void OgreModule::renderOneFrame(){
    m_ogreRoot->renderOneFrame();
}

void OgreModule::updateWindowDimensions()
{
    int w,h;
    SDL_GetWindowSize(OgreModule::getInstance().getSDLWindow(), &w, &h);
    if (w > 0 && h > 0)
    {
        m_ogreRenderWindow->resize(w, h);
        m_ogreRenderWindow->windowMovedOrResized();
    }
}

void OgreModule::setAmbientLight(const HUGO::Vector3& l){
    m_currentSceneManager->setAmbientLight(Ogre::ColourValue(l.x, l.y, l.z));
}

void OgreModule::setFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen(m_sdlWindow, fullscreen? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
        
void 
OgreModule::addResourceRecursive(const std::filesystem::path& path) {
    for(auto entry : directory_iterator(path)) {
        // Load directories
        if(entry.is_directory()) {
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                std::filesystem::canonical(entry.path()).string(),
                "FileSystem"
            );

            addResourceRecursive(entry.path());
        }

        // Load Zips
        else if(entry.is_regular_file() && entry.path().extension() == ".zip") {
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                std::filesystem::canonical(entry.path()).string(),
                "Zip"
            );
        }
    }
}

void 
OgreModule::loadResources()
{
    //load engine and game resources
    if(std::filesystem::exists(RESOURCES))
        addResourceRecursive(RESOURCES);

    if(std::filesystem::exists(ASSETS))
        addResourceRecursive(ASSETS);

    //initialize resources
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void
OgreModule::loadPlugins() {
    for(auto plugin : requiredPlugins)
        m_ogreRoot->loadPlugin(plugin);
}
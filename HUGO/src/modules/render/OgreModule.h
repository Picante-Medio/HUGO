#pragma once
#include <memory>

// Ogre tambien predeclara ogre root, pero en debug añade un namespace intermedio
// Ese namespace intermedio se añade mediante una macro y mete un using de dicho namespace,
// por eso podemos usar Ogre::Root sin tener que hacer Ogre::NamespaceDebug::Root, pero en 
// las predeclaraciones los usings no aplican y hay que poner todo el namespace, se podría hacer
// metiendo la macro pero me parece poco cómodo. Meted el include y palante
#include "OgreRoot.h"
#include <unordered_map>
#include "HugoTypes.h"
#include "HugoMath.h"

namespace Ogre{
    class OverlaySystem;
    class RenderWindow;
};

struct SDL_Window;

class OgreModule {

    //Not the name of the application window, but the internal name of the window inside Ogre.
    const char* OGRE_WINDOW_NAME = "HUGO Window";

    //Ogre Singletons. Can be obtained using ::getInstance() of the class.
    std::unique_ptr<Ogre::Root> m_ogreRoot;
    std::unique_ptr<Ogre::OverlaySystem> m_ogreOverlaySystem;
    
    Ogre::RenderWindow* m_ogreRenderWindow = nullptr;
    SDL_Window* m_sdlWindow = nullptr;
    

    std::unordered_map<HUGO::String, Ogre::SceneManager*> m_sceneManagers;

    Ogre::SceneManager* m_currentSceneManager;


    //Things required for creating default viewport
    Ogre::Viewport* m_viewport;
    Ogre::Camera* m_defaultCamera;
    Ogre::SceneManager* m_defaultCameraSceneManager;

    static OgreModule* instance;

    //Initializes Ogre Singletons and SDL window
    OgreModule();
    void loadResources();

    public:
    static OgreModule& getInstance();
    static void createInstance();
    static void destroyInstance();    

    //Sets the current sceneManager that scene systems will use
    void setCurrentSceneManager(HUGO::String sceneName);

    //Loads a sceneManager into memory. 
    void createSceneManager(HUGO::String sceneName);

    //Destroys a sceneManager. It will unload all of Ogres resources being used by it.
    void destroySceneManager(HUGO::String sceneName);

    Ogre::SceneManager* getCurrentSceneManager();
    Ogre::Viewport* getViewport();
    void viewportToDefaultCamera();

    //Destroys Ogre Singletons and removes SDL window
    ~OgreModule();

    void renderOneFrame();

    void updateWindowDimensions();

    Ogre::RenderWindow* getOgreRenderWindow() const;
    SDL_Window* getSDLWindow() const;

    void setAmbientLight(const HUGO::Vector3& l);
    void setFullscreen(bool fullscreen);
};
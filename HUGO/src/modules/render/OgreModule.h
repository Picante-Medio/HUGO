#pragma once
#include <memory>

// Ogre tambien predeclara ogre root, pero en debug añade un namespace intermedio
// Ese namespace intermedio se añade mediante una macro y mete un using de dicho namespace,
// por eso podemos usar Ogre::Root sin tener que hacer Ogre::NamespaceDebug::Root, pero en 
// las predeclaraciones los usings no aplican y hay que poner todo el namespace, se podría hacer
// metiendo la macro pero me parece poco cómodo. Meted el include y palante
#include "OgreRoot.h"

namespace Ogre{
    class OverlaySystem;
    class RenderWindow;
};

namespace HUGO {
    using OgreObjectID = int;
    constexpr OgreObjectID INVALID_OGRE_OBJECT_ID = std::numeric_limits<int>::max();
}

struct SDL_Window;

class OgreModule{

    //Not the name of the application window, but the internal name of the window inside Ogre.
    const char* OGRE_WINDOW_NAME = "HUGO Window";

    //Ogre Singletons. Can be obtained using ::getInstance() of the class.
    std::shared_ptr<Ogre::Root> m_ogreRoot;
    std::shared_ptr<Ogre::OverlaySystem> m_ogreOverlaySystem;
    
    Ogre::RenderWindow* m_ogreRenderWindow = nullptr;
    SDL_Window* m_sdlWindow = nullptr;
    
    //Initializes Ogre Singletons and SDL window
    OgreModule();
public:

    static OgreModule& Instance();

    //Destroys Ogre Singletons and removes SDL window
    ~OgreModule();

    void renderOneFrame();

    Ogre::RenderWindow* getOgreRenderWindow();
};
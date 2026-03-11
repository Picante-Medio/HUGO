#pragma once
#include <memory>

namespace Ogre{
    class Root;
    class OverlaySystem;
    class RenderWindow;
};

class SDL_Window;

class OgreModule{

    //Not the name of the application window, but the internal name of the window inside Ogre.
    const char* OGRE_WINDOW_NAME = "HUGO Window";

    //Ogre Singletons. Can be obtained using ::getInstance() of the class.
    std::shared_ptr<Ogre::Root> m_ogreRoot;
    std::shared_ptr<Ogre::OverlaySystem> m_ogreOverlaySystem;

    Ogre::RenderWindow* m_ogreRenderWindow = nullptr;
    SDL_Window* m_sdlWindow = nullptr;
public:
    OgreModule();
    //Initializes Ogre Singletons and SDL window
    void init();
    //Destroys Ogre Singletons and removes SDL window
    void destroy();

    void renderOneFrame();

    Ogre::RenderWindow* getOgreRenderWindow();
};
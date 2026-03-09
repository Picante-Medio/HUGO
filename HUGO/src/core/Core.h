#pragma once

#include <memory>

#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "BulletModule.h"

class Scene;
class VirtualTimer;
struct SDL_Window;

namespace HUGO {
    class ResourceManager;

    class Core {
        void initModules();

        //Ogre
        void initOgre();
        void destroyOgre();

        std::shared_ptr<Ogre::Root> m_ogreRoot;
        Ogre::RenderWindow* m_renderWindow;
        SDL_Window* sdlWindow;

        //Miniaudio
        void initMiniaudio();
        void destroyMiniaudio();
        std::shared_ptr<ma_engine> maEngine;

        // Physics
        void initPhysicsModule();
        void destroyPhysicsModule();
        std::shared_ptr<IPhysicsModule> physicsModule;

        VirtualTimer vt;
        std::shared_ptr<Scene> scenePtr;
        
        void locateResources();
        void loadResources();

        //Resource Manager
        std::shared_ptr<ResourceManager> m_resourceManager;

        // Lua
        void initLua();
    
    public:
        Core();
        void init();
        void destroy();

        void run();
    };

}

#pragma once

#include <memory>

#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "OgreOverlaySystem.h"
#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "BulletModule.h"
#include "OgreModule.h"
#include "MiniaudioModule.h"

class Scene;
class VirtualTimer;
struct SDL_Window;

namespace HUGO {
    class ResourceManager;

    class Core {
        const int FRAME_RATE = 10;
        void initModules();

        //Ogre
        std::shared_ptr<OgreModule> m_ogreModule;

        //Miniaudio
        std::shared_ptr<MiniaudioModule> m_miniaudioModule;

        

        // Physics
        void initPhysicsModule();
        void destroyPhysicsModule();
        std::shared_ptr<IPhysicsModule> physicsModule;

        VirtualTimer vt;
        std::shared_ptr<Scene> scenePtr;

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

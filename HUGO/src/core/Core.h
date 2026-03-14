#pragma once

#include <memory>

#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "OgreModule.h"
#include "MiniaudioModule.h"
#include "IPhysicsModule.h"

class Scene;
class VirtualTimer;
struct SDL_Window;

namespace HUGO {
    class ResourceManager;

    class Core {
        const int FRAME_RATE = 10;
        void initModules();

        //Ogre
        //std::shared_ptr<OgreModule> m_ogreModule;

        //Miniaudio
        //std::shared_ptr<MiniaudioModule> m_miniaudioModule;

        // Physics
        std::shared_ptr<IPhysicsModule> physicsModule;

        VirtualTimer vt;
        std::shared_ptr<Scene> scenePtr;

        //Resource Manager
        std::shared_ptr<ResourceManager> m_resourceManager;

    public:
        Core();
        void init();

        void run();
    };

}

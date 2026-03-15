#pragma once

#include <memory>

#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "OgreModule.h"
#include "MiniaudioModule.h"
#include "IPhysicsModule.h"

#include "SceneManager.h"

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

        SceneManager sceneManager;

    public:
        Core();
        void init();

        void run();
    };

}

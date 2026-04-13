#pragma once

#include <memory>

#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "OgreModule.h"
#include "MiniaudioModule.h"
#include "AudioAssetsModule.h"
#include "IPhysicsModule.h"
#include "HugoError.h"

#include "SceneManager.h"
#include "Input.h"

class InternalScene;
class VirtualTimer;
struct SDL_Window;

namespace HUGO {
    class ResourceManager;

    class Core {
        const uint32_t FIXED_STEP = 1000/60;
        void initModules();
        void initLua();
        void initAudioAssets();

        // Physics
        std::shared_ptr<IPhysicsModule> physicsModule;

        VirtualTimer vt;

        std::shared_ptr<Input> input;
        SceneManager sceneManager;

    public:
        Core();
        ErrorOrVoid init(int argc, char* argv[]);

        void run();
    };

}

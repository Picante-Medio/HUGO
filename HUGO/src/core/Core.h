#pragma once

#include <memory>

#include "HugoVirtualTimer.h"
#include "miniaudio.h"
#include "OgreModule.h"
#include "MiniaudioModule.h"
#include "AudioAssetsModule.h"
#include "IPhysicsModule.h"
#include "HugoError.h"
#include "LuaBinder.h"

#include "SceneManager.h"
#include "Input.h"
#include "GameLibrary.h"

class InternalScene;
class VirtualTimer;
struct SDL_Window;

namespace HUGO {
    class ResourceManager;

    class Core {
        const int32_t FIXED_STEP = 1000/60;
        void initLua();

        VirtualTimer vt;

        std::unique_ptr<Input> input;
        std::unique_ptr<IPhysicsModule> physicsModule;

        std::unique_ptr<SceneManager> sceneManager;
        
        std::string gameName;
        std::string chatsDirectory;
        std::string initialScene;
        bool fullscreen = false;

        std::unique_ptr<HUGO::GameLibrary> gameInstance;

        /// Constructions

        /// @brief First construction step
        //
        /// Reads hugo.config file. Throws exception if process goes wrong
        void processHugoConfig();

        /// @brief Second construction step
        ///
        /// Instantiates all dependency modules
        void initializeDependencies();

        /// @brief Third construction step
        ///
        /// Dynamically load the game library
        void loadGameLibrary();

        /// @brief Fourth construction step
        ///
        /// Creates the scene manager with the scene data generators of each module
        void loadSceneManager();


        /// Destruction (reversed order)

        /// @brief First destruction step
        ///
        /// Creates the scene manager with the scene data generators of each module
        void destroySceneManager();

        /// @brief Second destruction step
        ///
        /// Dynamically load the game library
        void unloadGameLibrary();

        /// @brief Third destruction step
        ///
        /// Instantiates all dependency modules
        void destroyDependencies();


    public:
        Core();
        ~Core();

        HUGO::ErrorOrVoid run();
    };

}

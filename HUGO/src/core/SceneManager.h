#pragma once
#include <memory>

#include <unordered_map>
#include "ChatParser.h"

#include "HugoError.h"
#include "EcsTypes.h"
#include "HugoMath.h"
#include <vector>

#ifdef __linux__
#include <stdint.h>
#endif

class InternalScene;
class IPhysicsModule;

namespace HUGO {
    class IInput;

    class SceneManager {

        HUGO::ChatParser m_chatParser;
        //Scene map that stores created game scenes.
        std::unordered_map<std::string, std::shared_ptr<InternalScene>> m_sceneMap;

        //Pointer to a scene inside m_sceneMap. It is the scene that will be updated in the main loop.
        std::weak_ptr<InternalScene> m_currentScene;
        std::string m_currentSceneName;

        //Removes a scene from m_sceneMap, to free space that is not used.
        HUGO::ErrorOrVoid destroyScene(const std::string& sceneName);

        //Reads chatParser, creates a scene based on the SceneData, and indexes it in m_sceneMap.
        HUGO::ErrorOrVoid createScene(const std::string& sceneName);

        //Physics module
        IPhysicsModule& m_physicsModule;

        //Input module
        HUGO::IInput& m_input;

        //Checkers for scene-changing requests
        struct SceneQuery{
            enum class QueryType{
                CHANGE_SCENE, DELETE_SCENE, LOAD_SCENE
            };

            QueryType queryType;
            std::string sceneName;
        };

        std::vector<SceneQuery> m_sceneQueries;

        bool quit = false;
    public:
        //Initializes the parser with chat folder and sets physicsModule and inputModule
        SceneManager(HUGO::IInput& inputModule, const std::string& chatFolder,IPhysicsModule& physicsModule);

        //Unloads scene from memory
        void loadScene(const std::string& sceneName);

        //Unloads scene from memory
        void unloadScene(const std::string& sceneName);

        /*
        Requests SceneManager to change to scene [sceneName].
        It requests it because the current update cycle needs to finish before m_currentScene can change.
        */
        void setCurrentScene(const std::string& sceneName);
        void setCurrentSceneAndDestroyPrevious(const std::string& sceneName);
        
        void setCurrentSceneLUA(const std::string sceneName);

        //Called by HUGO::Core. If there is a request for a scene change, change m_currentScene
        HUGO::ErrorOrVoid manageQueries();

        //---Updates and fixedUpdates of current Scene
        void updateCurrentScene(uint32_t dt);
        void fixedUpdateCurrentScene(uint32_t dt);
        void dispatchMessagesCurrentScene();

        HUGO::ErrorOrType<EntityID> instantiatePrefab(const std::string& prefabName, std::shared_ptr<InternalScene> scene);

        inline bool getQuit() const { return quit; }
        inline void setQuit(bool newQuit) { quit = newQuit; }
        
        void setAmbientLight(const HUGO::Vector3& l);
    };
}

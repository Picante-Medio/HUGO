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
        std::unordered_map<HUGO::String, std::shared_ptr<InternalScene>> m_sceneMap;

        //Pointer to a scene inside m_sceneMap. It is the scene that will be updated in the main loop.
        std::weak_ptr<InternalScene> m_currentScene;
        HUGO::String m_currentSceneName;

        //Removes a scene from m_sceneMap, to free space that is not used.
        HUGO::ErrorOrVoid unloadScene(const HUGO::String& sceneName);

        //Physics module
        std::shared_ptr<IPhysicsModule> m_physicsModule;

        //Input module
        std::shared_ptr<HUGO::IInput> m_input;

        //Checkers for scene-changing requests
        struct SceneQuery{
            enum class QueryType{
                CHANGE_SCENE, DELETE_SCENE
            };

            QueryType queryType;
            HUGO::String sceneName;
        };

        std::vector<SceneQuery> m_sceneQueries;

        bool quit = false;
    public:
        //initializeParser() must be called after this.
        SceneManager(std::shared_ptr<HUGO::IInput> inputModule);

        //Initializes the parser with chat folder and sets physicsModule
        HUGO::ErrorOrVoid initializeParser(const HUGO::String& chatFolder, std::shared_ptr<IPhysicsModule> physicsModule);

        //Reads chatParser, creates a scene based on the SceneData, and indexes it in m_sceneMap.
        HUGO::ErrorOrVoid loadScene(const HUGO::String& sceneName);

        /*
        Requests SceneManager to change to scene [sceneName].
        It requests it because the current update cycle needs to finish before m_currentScene can change.
        */
        void setCurrentScene(const HUGO::String& sceneName);
        void setCurrentSceneAndDestroyPrevious(const HUGO::String& sceneName);
        
        void setCurrentSceneLUA(const HUGO::String sceneName);

        //Called by HUGO::Core. If there is a request for a scene change, change m_currentScene
        HUGO::ErrorOrVoid manageQueries();

        //---Updates and fixedUpdates of current Scene
        void updateCurrentScene(uint32_t dt);
        void fixedUpdateCurrentScene(uint32_t dt);
        void dispatchMessagesCurrentScene();

        HUGO::ErrorOrType<EntityID> instantiatePrefab(const HUGO::String& prefabName, std::shared_ptr<InternalScene> scene);

        inline bool getQuit() const { return quit; }
        inline void setQuit(bool newQuit) { quit = newQuit; }
        
        void setAmbientLight(const HUGO::Vector3& l);
    };

}

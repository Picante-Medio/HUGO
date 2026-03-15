#pragma once
#include <memory>

#include <unordered_map>
#include "ChatParser.h"

#include "HugoError.h"

class Scene;
class IPhysicsModule;
class SceneManager{
    HUGO::ChatParser m_chatParser;
    //Scene map that stores created game scenes.
    std::unordered_map<HUGO::String, std::shared_ptr<Scene>> m_sceneMap;

    //Pointer to a scene inside m_sceneMap. It is the scene that will be updated in the main loop.
    std::shared_ptr<Scene> m_currentScene = nullptr;

    //Reads chatParser, creates a scene based on the SceneData, and indexes it in m_sceneMap.
    ErrorOrVoid loadScene(const HUGO::String& sceneName);
    //Removes a scene from m_sceneMap, to free space that is not used.
    ErrorOrVoid unloadScene(const HUGO::String& sceneName);

    //Physics module
    std::shared_ptr<IPhysicsModule> m_physicsModule;
public:
    //initializeParser() must be called after this.
    SceneManager();

    //Initializes the parser with chat folder and sets physicsModule
    ErrorOrVoid initializeParser(const HUGO::String& chatFolder, std::shared_ptr<IPhysicsModule> physicsModule);

    /*
    Starts scene named sceneName.
    If a scene were currently running, it will stop running and will be kept in memory for future use.
    */
    void setCurrentScene(const HUGO::String& sceneName);
    void setCurrentSceneLUA(const HUGO::String sceneName);

    void updateCurrentScene(double dt);
    void fixedUpdateCurrentScene(double dt);
};
#pragma once

#include <memory>
#include <queue>

#include "EcsTypes.h"
#include "System.h"
#include "HugoTypes.h"
#include "HugoError.h"

#include "IScene.h"
#include "IInput.h"

class Input;
namespace HUGO {
    class SceneManager;
}
class InternalScene : public HUGO::IScene, public std::enable_shared_from_this<InternalScene>
{
    std::vector<std::shared_ptr<HUGO::System>> systems;
    std::queue<HUGO::StringError> errorsToLog;
    std::queue<std::string> stringsToLog;

    HUGO::IInput& input;
    HUGO::SceneManager& sceneManager;

    bool hasComponent(EntityID entity, ComponentID componentID);
    void removeComponentAux(EntityID entity, ComponentID componentID, DenseListIndex index);
public:
    InternalScene(HUGO::IInput& input, HUGO::SceneManager& sceneManager);

    //Calls load() on all systems. Must be used only by SceneManager
    void load();

    //Calls init() on all systems. Must be used only by SceneManager
    void init();

    //Calls exit() on all systems. Must be used only by SceneManager
    void exit();

    inline void addSystem(std::shared_ptr<HUGO::System> system) { systems.push_back(system); }

    // Logic update
    void update(uint32_t dt);

    // Physics update
    void fixedUpdate(uint32_t dt);

    // Entity handling
    EntityID createEntity() override;

    // LUA console stuff
    int createEntityLUA();

    void addComponentLUA(int entityID, std::string &componentName);

    void removeComponentLUA(int entityID, std::string &componentName);

    void removeEntity(EntityID entity) override;
    void removeEntityLUA(int entity);

    void printEntity(int id);
    void printEntities();

    void log(const HUGO::StringError& error) override;
    void log(const std::string& log) override;

    HUGO::ErrorOrType<std::string> getNextError() override;
    HUGO::ErrorOrType<std::string> getNextLog() override;

    void dispatchMessages() override;

    HUGO::IInput& getInput() override;

    void changeScene(const std::string& sceneName) override;

    void changeAndDestroyPreviousScene(const std::string& sceneName) override;

    HUGO::ErrorOrType<EntityID> instantiatePrefab(const std::string& prefabName) override;

    void exitGame() override;
    void changeAmbientLight(const HUGO::Vector3& l) override;

    void loadScene(const std::string& sceneName) override;
    void unloadScene(const std::string& sceneName) override;

    using IScene::hasComponent; 
};

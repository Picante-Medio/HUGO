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
    std::queue<std::shared_ptr<Error>> errorsToLog;
    std::queue<HUGO::String> stringsToLog;

    std::shared_ptr<HUGO::IInput> input;
    HUGO::SceneManager& sceneManager;

    void removeComponentAux(EntityID entity, ComponentID componentID, DenseListIndex index);
public:
    InternalScene(std::shared_ptr<HUGO::IInput> input, HUGO::SceneManager& sceneManager);

    //Calls load() on all systems. Must be used only by SceneManager
    void load();

    //Calls init() on all systems. Must be used only by SceneManager
    void init();

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

    void log(std::shared_ptr<Error> error) override;
    void log(HUGO::String log) override;

    ErrorOrType<HUGO::String> getNextError() override;
    ErrorOrType<HUGO::String> getNextLog() override;

    void dispatchMessages() override;

    std::shared_ptr<HUGO::IInput> getInput() override;

    void changeScene(const HUGO::String& sceneName) override;

    void changeAndDestroyPreviousScene(const HUGO::String& sceneName) override;
};

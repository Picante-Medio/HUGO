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
class Scene : public HUGO::IScene, public std::enable_shared_from_this<Scene>
{
    std::vector<std::shared_ptr<HUGO::System>> systems;
    std::queue<std::shared_ptr<Error>> errorsToLog;
    std::queue<HUGO::String> stringsToLog;

    std::shared_ptr<HUGO::IInput> input;

    void removeComponentAux(EntityID entity, ComponentID componentID, DenseListIndex index);
public:
    Scene(std::shared_ptr<HUGO::IInput> input);

    void load();

    inline void addSystem(std::shared_ptr<HUGO::System> system) { systems.push_back(system); }

    // Logic update
    void update(double dt);

    // Physics update
    void fixedUpdate(double dt);

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
};

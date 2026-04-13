#pragma once

#include <memory>
#include <unordered_map>
#include <string>

class InternalScene;
class IPhysicsSimulation;

class IPhysicsModule {
protected:
  std::unordered_map<std::string, std::shared_ptr<IPhysicsSimulation>> activeSimulations;
  std::unordered_map<std::string, std::shared_ptr<IPhysicsSimulation>> inactiveSimulations;

public:
    explicit IPhysicsModule() {}
    virtual ~IPhysicsModule() {}

    virtual void createSceneSimulation(std::string sceneName, std::shared_ptr<InternalScene> scene) = 0;
    virtual void destroySceneSimulation(std::string sceneName) = 0;

    virtual void resumeSceneSimulation(std::string sceneName) = 0;
    virtual void pauseSceneSimulation(std::string sceneName) = 0;

    // sync info and step the simulation of active scenes
    virtual void performSimulations(uint32_t dt) = 0;

    // physics visual debugger
    virtual void debugDraw() = 0;
};

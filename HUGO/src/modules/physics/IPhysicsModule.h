#pragma once

#include <memory>
#include <vector>

class Scene;
class IPhysicsSimulation;

class IPhysicsModule {
protected:
  std::vector<std::shared_ptr<IPhysicsSimulation>> activeSimulations;
  std::vector<std::shared_ptr<IPhysicsSimulation>> inactiveSimulations;

  // index of currentScene
  int iScene;

public:
    explicit IPhysicsModule() {}
    virtual ~IPhysicsModule() {}

    virtual void createSceneSimulation(std::shared_ptr<Scene> scene) = 0;
    virtual void destroySceneSimulation(std::shared_ptr<Scene> scene) = 0;

    virtual void resumeSceneSimulation(std::shared_ptr<Scene> scene) = 0;
    virtual void pauseSceneSimulation(std::shared_ptr<Scene> scene) = 0;

    // sync info and step the simulation of active scenes
    virtual void performSimulations(double dt) = 0;
};

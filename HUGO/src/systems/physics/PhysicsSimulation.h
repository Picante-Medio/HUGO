#pragma once

#include <memory>

class Scene;

class PhysicsSimulation {
protected:
  std::shared_ptr<Scene> scene = nullptr;

public:
    explicit PhysicsSimulation(std::shared_ptr<Scene> scene) : scene(scene) {}
    virtual ~PhysicsSimulation() {}
    virtual void* getSimulation() = 0;
    virtual void stepSimulation(double dt) = 0;
};

#pragma once

#include <memory>

class Scene;

class IPhysicsSimulation {
protected:
    std::shared_ptr<Scene> scene;

public:
    explicit IPhysicsSimulation(std::shared_ptr<Scene> scene) : scene(scene) {}
    virtual ~IPhysicsSimulation() {}

    virtual void destroySimulation() = 0;

    // sync info and step the simulation
    virtual void performSimulation(double dt) = 0;
};
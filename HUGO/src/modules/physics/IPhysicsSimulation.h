#pragma once

#include <memory>

class InternalScene;

class IPhysicsSimulation {
protected:
    std::shared_ptr<InternalScene> scene;

public:
    explicit IPhysicsSimulation(std::shared_ptr<InternalScene> scene) : scene(scene) {}
    virtual ~IPhysicsSimulation() {}

    virtual void destroySimulation() = 0;

    // sync info and step the simulation
    virtual void performSimulation(double dt) = 0;

    // physics visual debugger
    virtual void debugDraw() = 0;

    std::shared_ptr<InternalScene> getScene() { return scene; }
};
#pragma once

#include <memory>

class InternalScene;

class IPhysicsSimulation {
protected:
    InternalScene& scene;

public:
    explicit IPhysicsSimulation(InternalScene& scene) : scene(scene) {}
    virtual ~IPhysicsSimulation() {}

    virtual void destroySimulation() = 0;

    // sync info and step the simulation
    virtual void performSimulation(double dt) = 0;

    // physics visual debugger
    virtual void debugDraw() = 0;

    InternalScene& getScene() { return scene; }
};
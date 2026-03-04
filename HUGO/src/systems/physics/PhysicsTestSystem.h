#pragma once

#include "System.h"

class PhysicsTestSystem : public System {

public:
    PhysicsTestSystem(std::shared_ptr<Scene> scene);
    ~PhysicsTestSystem();

    void fixedUpdate(double dt);
    void update(double dt);
};
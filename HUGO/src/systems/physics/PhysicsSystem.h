#pragma once

#include "Ecs.h"
#include "System.h"

class PhysicsSystem : public System {

    public:
    PhysicsSystem(std::shared_ptr<Scene> scene);

    void update(double dt) override;
    void fixedUpdate(double dt) override;
};
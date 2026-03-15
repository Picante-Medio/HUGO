#pragma once

#include "HugoSystemMacros.h"

HSYSTEM(PhysicsTestSystem)

public:
    ~PhysicsTestSystem();

    void init();
    void onLoad();

    void fixedUpdate(double dt);
    void update(double dt);
END_HSYSTEM()
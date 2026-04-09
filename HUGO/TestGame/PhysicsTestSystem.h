#pragma once

#include "HUGO.h"

HSYSTEM(PhysicsTestSystem)

    EntityID staticBase = 0;

public:
    ~PhysicsTestSystem();

    void init();
    void onLoad();

    void fixedUpdate(double);
    void update(double);
END_HSYSTEM()
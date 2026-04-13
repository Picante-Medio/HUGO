#pragma once

#include "HUGO.h"

HSYSTEM(PhysicsTestSystem)

    EntityID staticBase = 0;

public:
    ~PhysicsTestSystem();

    void init();
    void onLoad();

    void fixedUpdate(uint32_t);
    void update(uint32_t);
END_HSYSTEM()
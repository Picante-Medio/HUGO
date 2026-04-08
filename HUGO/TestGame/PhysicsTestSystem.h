#pragma once

#include "HUGO.h"

HSYSTEM(PhysicsTestSystem)

public:
    ~PhysicsTestSystem();

    void init();
    void onLoad();

    void fixedUpdate(double);
    void update(double);
END_HSYSTEM()
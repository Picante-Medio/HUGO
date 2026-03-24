#pragma once
#include "HugoSystemMacros.h"

HSYSTEM(GizmoSystem)
public:
    void init();

    void onLoad();

    void update(double dt);

    void fixedUpdate(double dt);

END_HSYSTEM()


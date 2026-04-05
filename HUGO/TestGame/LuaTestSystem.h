#pragma once 

#include "HUGO.h"
#include "HugoSystemMacros.h"
#include <stdexcept>

HSYSTEM(LuaTestSystem)
    double currentTime = 0.;
    double nextTime = 3.;

    void foo();
public:
    void init() override {}
    void onLoad() override;
    void fixedUpdate(double) override {}
    void update(double dt) override;
END_HSYSTEM()

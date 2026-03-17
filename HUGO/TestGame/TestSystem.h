#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(TestSystem)
public:
    void init() override;
    //Called every time the scene is loaded
    void onLoad() override;

    void fixedUpdate(double dt) override;
    void update(double dt) override;

END_HSYSTEM()
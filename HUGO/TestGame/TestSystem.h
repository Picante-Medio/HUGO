#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(TestSystem)
    //test
    EntityID e1;
    EntityID e2;
    EntityID e3;
public:
    void init() override;
    //Called every time the scene is loaded
    void onLoad() override;

    void fixedUpdate(uint32_t dt) override;
    void update(uint32_t dt) override;

END_HSYSTEM()
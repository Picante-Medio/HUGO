#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(TestSystemVideo1)
    //test
    EntityID menuFondo;
    EntityID menuText;
    EntityID menuButton;
    EntityID audioSlider;
    EntityID audio;
public:
    void init() override;
    //Called every time the scene is loaded
    void onLoad() override;

    void fixedUpdate(uint32_t dt) override;
    void update(uint32_t dt) override;

END_HSYSTEM()
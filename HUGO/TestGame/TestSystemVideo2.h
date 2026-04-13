#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(TestSystemVideo2)
    //test
    EntityID menuButton;
    EntityID sinbad;
    EntityID suelo;

    void createCube(EntityID ent);
public:
    void init() override;
    //Called every time the scene is loaded
    void onLoad() override;

    void fixedUpdate(uint32_t) override;
    void update(uint32_t dt) override;

END_HSYSTEM()
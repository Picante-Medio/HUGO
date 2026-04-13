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

    void fixedUpdate(double) override;
    void update(double dt) override;

END_HSYSTEM()
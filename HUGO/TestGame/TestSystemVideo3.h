#pragma once

#include "EcsTypes.h"
#include "HUGO.h"
#include "System.h"

HSYSTEM(TestSystemVideo3)
    EntityID cuboPedro;
    EntityID cuboPablo;
    EntityID sinabd;

    const double rotationFactor = .2;
public:
    void init() override;
    //Called every time the scene is loaded
    void onLoad() override;

    void fixedUpdate(double dt) override;
    void update(double dt) override;

END_HSYSTEM()

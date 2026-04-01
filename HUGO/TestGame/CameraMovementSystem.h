#pragma once

#include "HUGO.h"

//A TEST CLASS!!!!!!!!!!
HSYSTEM(CameraMovementSystem)

public:
    void init() override;
    void onLoad() override;

    void fixedUpdate(double dt) override;
    void update(double dt) override;
END_HSYSTEM()


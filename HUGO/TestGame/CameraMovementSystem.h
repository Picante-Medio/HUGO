#pragma once

#include "HUGO.h"

HSYSTEM(CameraMovementSystem)

public:
    void init() override;
    void onLoad() override;

    void fixedUpdate(double dt) override;
    void update(double dt) override;
END_HSYSTEM()


#pragma once
#include "HugoSystemMacros.h"

HSYSTEM(TweenSystem)

public:
    void init() override;
    void onLoad() override;
    void update(double dt) override;
    void fixedUpdate(double) override;

END_HSYSTEM()

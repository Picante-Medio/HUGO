#pragma once
#include "HugoSystemMacros.h"

HSYSTEM(TweenSystem)

public:
    void init() override;
    void onLoad() override;
    void update(uint32_t dt) override;
    void fixedUpdate(uint32_t) override;

END_HSYSTEM()

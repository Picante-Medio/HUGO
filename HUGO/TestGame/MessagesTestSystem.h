#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(MessagesTestSystem) 
    double currT = 0.;
    double nextT = 8000.;

public:
    void init() override;
    void onLoad() override;

    void fixedUpdate(uint32_t dt) override;
    void update(uint32_t dt) override;

END_HSYSTEM()

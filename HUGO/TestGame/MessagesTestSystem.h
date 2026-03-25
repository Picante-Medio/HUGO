#pragma once

#include "HUGO.h"
#include "System.h"

HSYSTEM(MessagesTestSystem) 
    double currT = 0.;
    double nextT = 0.;
public:
    void init() override;
    void onLoad() override;

    void fixedUpdate(double dt) override;
    void update(double dt) override;

END_HSYSTEM()
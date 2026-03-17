#pragma once

#include "HugoSystemMacros.h"

HSYSTEM(LuaSystem)

    void Print(std::string msg);

public:

   void update(double dt) override;
   void fixedUpdate(double dt) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
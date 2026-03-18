#pragma once

#include "HugoSystemMacros.h"

HSYSTEM(ConsoleSystem)

    std::deque<HUGO::String> history;

    void PrintLogs();
    void PrintErrors();
    
public:

   void update(double dt) override;
   void fixedUpdate(double dt) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
#pragma once

#include "HugoSystemMacros.h"

class TextComponent;

HSYSTEM(ConsoleSystem)

    std::deque<HUGO::String> history;
    EntityID console;

    void printLogs(TextComponent& consoleText);
    void printErrors(TextComponent& consoleText);

    void receiveInput(const HUGO::String& input);
    
public:

   void update(double dt) override;
   void fixedUpdate(double dt) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
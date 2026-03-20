#pragma once

#include "HugoSystemMacros.h"

class TextComponent;

HSYSTEM(ConsoleSystem)

    std::deque<HUGO::String> history;
    std::variant<int, float, bool, HUGO::String> lastValueReturned;
    EntityID console;

    void printLogs(TextComponent& consoleText);
    void printErrors(TextComponent& consoleText);

    void receiveInput(const HUGO::String& input);

    template<typename T>
    void handleReturn(ErrorOrType<T>& returnedInfo){

        if(returnedInfo.isError()) scene->log(std::make_shared<Error>("LUA: " + returnedInfo.getError()));
        else lastValueReturned = returnedInfo.getValue();
    }

    
public:

   void update(double) override;
   void fixedUpdate(double) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
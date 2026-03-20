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
    void handleReturn(const ErrorOrType<T>& returnedInfo){

        if(returnedInfo.isError()) scene->log(std::make_shared<Error>(returnedInfo.error));
        else{
            
            //if constexpr (std::is_same_v<returnedInfo.info, HUGO::Int> || std::is_same_v<T, int>)
                
            //else if constexpr(std::is_same_v<T, HUGO::Float> || std::is_same_v<T, double> || std::is_same_v<T, float>)
            //    return (HUGO::Float)luaL_checknumber(lua, idx);
            //else if constexpr (std::is_same_v<T, bool>)
            //    return lua_toboolean(lua, idx);
            //else if constexpr (std::is_same_v<T, HUGO::String> || std::is_same_v<T, std::string>)
            //    return HUGO::String(luaL_checkstring(lua, idx));
            
            //    lastValueReturned = returnedInfo.info;
        }
    }

    
public:

   void update(double) override;
   void fixedUpdate(double) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
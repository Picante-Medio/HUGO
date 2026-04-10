#pragma once

#include "HugoSystemMacros.h"
#include "HugoMath.h"
#include "LuaBinder.h"

#include <queue>

struct TextComponent;

HSYSTEM(ConsoleSystem)

std::vector<HUGO::String> history;
size_t historyIndex; // Constant access and reset
EntityID consoleInput;
EntityID consoleText;
EntityID consoleInfo;

HUGO::String logIni = ">  ";
bool active; // to set weather the console is active or not

static inline std::queue<HUGO::String> luaOutput = std::queue<HUGO::String>();

    static int consoleLuaPrint(lua_State* lua){
    
        if(!lua_isstring(lua, -1)){
            ConsoleSystem::luaOutput.push("HUGO wasn't able to interpret the lua value used in print as a string. If you used \"print()\" try using \"print(\"\")\" instead");
            return 0;
        }

        HUGO::String out = HUGO::String(lua_tostring(lua, -1));
        lua_pop(lua, -1);
        ConsoleSystem::luaOutput.push(out);

        return 0;
    }

    void printLuaOutput(TextComponent& consoleText);
    void printLogs(TextComponent& consoleText);
    void printErrors(TextComponent& consoleText);

    HUGO::String getNextHistoric();
    HUGO::String getPreviousHistoric();

    void receiveInput(const HUGO::String& input);
    void handleKeyInput();

    template<typename T>
    void handleReturn(ErrorOrType<T>& returnedInfo){

        std::variant<int, float, bool, HUGO::String>  lastValueReturned;

        if(returnedInfo.isError()) scene->log(std::make_shared<StringError>(returnedInfo.getError()));
        else lastValueReturned = returnedInfo.getValue();

        if(std::holds_alternative<HUGO::Int>(lastValueReturned)){
            scene->log(std::to_string(std::get<HUGO::Int>(lastValueReturned)) + "\n");
        }
        else if(std::holds_alternative<HUGO::Float>(lastValueReturned)){
            scene->log(std::to_string(std::get<HUGO::Int>(lastValueReturned)) + "\n");
        }
        else if(std::holds_alternative<bool>(lastValueReturned)){
            bool value = std::get<bool>(lastValueReturned);
            if(value) scene->log("true\n");
            else scene->log("false\n");
        }
        else{
            scene->log(std::get<HUGO::String>(lastValueReturned) + "\n");
        }
    }

public:

   void update(double) override;
   void fixedUpdate(double) override;

   void onLoad() override;
   void init() override;

END_HSYSTEM()
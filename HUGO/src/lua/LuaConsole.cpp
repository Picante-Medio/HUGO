#include "LuaConsole.h"
#include <iostream>

void LuaConsole::Print(std::string msg) {
    std::cout << msg;
    //TO_CHAT: Print on game's console which is not created yet
}
#include "HugoTypes.h"
#include <string>

class LuaConsole
{
public:
    LuaConsole() {}
    ~LuaConsole() {}

    void Print(std::string msg);
};

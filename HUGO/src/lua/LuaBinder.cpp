#include "LuaBinder.h"
 
LuaBinder &LuaBinder::Instance() {
    static LuaBinder instance;
    return instance;
}

LuaBinder::~LuaBinder() {
    lua_close(lua);
}

void
LuaBinder::Init() {
    lua = luaL_newstate();
    luaL_openlibs(lua);
}

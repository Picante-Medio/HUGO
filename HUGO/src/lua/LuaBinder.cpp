#include "LuaBinder.h"
#include "HugoError.h"
#include "HugoTypes.h"

LuaBinder &LuaBinder::Instance() {
  static LuaBinder instance;
  return instance;
}

LuaBinder::~LuaBinder() { lua_close(lua); }

void LuaBinder::Init() {
  lua = luaL_newstate();
  luaL_openlibs(lua);
}

ErrorOrVoid LuaBinder::runFile(HUGO::String path) {
  // If path is invalid
    if (luaL_loadfile(lua, path.c_str()) != LUA_OK) {
        StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return ErrorOrVoid(error);
    }

    // If lua crashes on runtime
    if (lua_pcall(lua, 0, 0, 0) != LUA_OK) {
        StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return ErrorOrVoid(error);
    }

    // Otherwise
    return ErrorOrVoid();
}

ErrorOrVoid
LuaBinder::interpretString(HUGO::String command) {
    //if string is invalid
    if (luaL_dostring(lua, command.c_str()) != LUA_OK) {
        StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return ErrorOrVoid(error);
    }

    //Otherwise
    return ErrorOrVoid();
}

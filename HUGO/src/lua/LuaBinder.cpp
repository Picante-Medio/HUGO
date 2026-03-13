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
int LuaBinder::setLuaPath(HUGO::String path )
{
    lua_getglobal( lua, "package" );
    lua_getfield( lua, -1, "path" ); // get field "path" from table at top of stack (-1)
    std::string cur_path = lua_tostring( lua, -1 ); // grab path string from top of stack
    cur_path.append( ";" ); // do your path magic here
    cur_path.append( path.c_str() );
    lua_pop( lua, 1 ); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring( lua, cur_path.c_str() ); // push the new one
    lua_setfield( lua, -2, "path" ); // set the field "path" in table at -2 with value at top of stack
    lua_pop( lua, 1 ); // get rid of package table from top of stack
    return 0; // all done!
}
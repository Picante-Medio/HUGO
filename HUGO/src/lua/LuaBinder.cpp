#include "LuaBinder.h"
#include "HugoError.h"
#include "HugoTypes.h"

#include <filesystem>

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
    HUGO::String fullPath = basePath + path;
  // If path is invalid
    if (luaL_loadfile(lua, fullPath.c_str()) != LUA_OK) {
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

void LuaBinder::setLuaPath(HUGO::String path )
{
    lua_getglobal(lua, "package");
    lua_getfield(lua, -1, "path");

    const char* cur_path = lua_tostring(lua, -1);
    std::string new_path = cur_path ? cur_path : "";

    new_path += ";";
    new_path += path.c_str();

    lua_pop(lua, 1); // pop old path

    lua_pushstring(lua, new_path.c_str());
    lua_setfield(lua, -2, "path");

    lua_pop(lua, 1); // pop package
}


void 
LuaBinder::setScriptPath(HUGO::String path) {
    basePath = path;
}

void
LuaBinder::requireFolder(HUGO::String path) {
#ifdef __linux
    setLuaPath(path + "/?.lua");
#else
    setLuaPath(path + "\\?.lua");
#endif
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            HUGO::String moduleName = entry.path().stem().string();
            interpretString("require '" + moduleName + "'");
        }
    }
}

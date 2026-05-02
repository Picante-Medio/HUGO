#include "LuaBinder.h"

#include <filesystem>

#include "HugoError.h"
#include "HugoTypes.h"

LuaBinder::LuaBinder() {
    lua = luaL_newstate();
    luaL_openlibs(lua);
}

LuaBinder::~LuaBinder() { lua_close(lua); }

LuaBinder&
LuaBinder::getInstance() {
    static LuaBinder instance = LuaBinder();
    return instance;
}

HUGO::ErrorOrVoid
LuaBinder::interpretString(HUGO::String command) {
    //if string is invalid
    if (luaL_dostring(lua, command.c_str()) != LUA_OK) {
        HUGO::StringError error = HUGO::StringError(HUGO::String(lua_tostring(lua, -1)));
        lua_pop(lua, 1);
        return HUGO::ErrorOrVoid(error);
    }

    //Otherwise
    return HUGO::ErrorOrVoid();
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
LuaBinder::setPrintFunction(lua_CFunction nPrint){
    lua_register(lua, "print", nPrint);
}

void
LuaBinder::requireFolder(HUGO::String path) {
    std::filesystem::path fsPath(path);
    std::filesystem::path luaPath = fsPath / "?.lua";

    setLuaPath(luaPath.string());

    for (const auto& entry : std::filesystem::directory_iterator(fsPath)) {
        if (entry.is_regular_file()) {
            HUGO::String moduleName = entry.path().stem().string();
            interpretString("require '" + moduleName + "'");
        }
    }
}

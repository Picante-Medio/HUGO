#pragma once

#include "HugoError.h"
#include "HugoMath.h"
#include "HugoTypes.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <luaconf.h>
#include <type_traits>
#include <utility>

//Functions to use the right lua types for arguments and return types
#pragma region HelperFunctions
template<typename T>
T lua_get(lua_State* lua, int idx) {
    if constexpr (std::is_same_v<T, HUGO::Int> || std::is_same_v<T, int>)
        return (HUGO::Int)luaL_checknumber(lua, idx);

    else if constexpr (std::is_same_v<T, HUGO::Float> || std::is_same_v<T, double> || std::is_same_v<T, float>)
        return (HUGO::Float)luaL_checknumber(lua, idx);

    else if constexpr (std::is_same_v<T, bool>)
        return lua_toboolean(lua, idx);

    else if constexpr (std::is_same_v<T, HUGO::String> || std::is_same_v<T, std::string>)
        return luaL_checkstring(lua, idx);
    else
        static_assert(sizeof(T) == 0, "lua_get: unsupported type");
}

template<typename T>
static void lua_push_result(lua_State* lua, T value) {
    if constexpr (std::is_same_v<T, HUGO::Int> || std::is_same_v<T, int>)
        lua_pushinteger(lua, value);

    else if constexpr (std::is_same_v<T, HUGO::Float> || std::is_same_v<T, double> || std::is_same_v<T, float>)
        lua_pushnumber(lua, value);

    else if  constexpr (std::is_same_v<T, bool>)
        lua_pushboolean(lua, value);

    else if constexpr (std::is_same_v<T, HUGO::String> || std::is_same_v<T, std::string>)
        lua_pushstring(lua, value.c_str());

    else 
        throw "You should only declare functions with parameters of basic types that lua can read";
}
#pragma endregion

#pragma region error
struct LuaRuntimeError : Error {
    HUGO::String error;
    HUGO::String description() override { return error; }

    LuaRuntimeError(HUGO::String errorDesc) : error(errorDesc) {}
};
#pragma endregion

class LuaBinder {
    //pointer to the lua interpeter instance
    lua_State* lua;
    HUGO::String lastError;

    template<typename ReturnType, typename ...ArgTypes, std::size_t... I>
    static int callLuaFunc(lua_State* lua, ReturnType(*func)(ArgTypes...), std::index_sequence<I...>) {
        if constexpr (std::is_same_v<ReturnType, void>) {
            func(lua_get<ArgTypes>(lua, I + 1)...);
            return 0; 
        }
        else {
            ReturnType result = func(lua_get<ArgTypes>(lua, I + 1)...);
            lua_push_result(lua, result);
            return 1;
        }
    }

public:
    static LuaBinder &Instance();
    ~LuaBinder();

    //Init must be called before using lua
    void Init();

    /** 
    * @param function function to call in lua
    * @param luaName name of the lua call to the function
    */
    template<typename ReturnType, typename ...ArgTypes>
    void registerFunc(ReturnType(*function)(ArgTypes...), HUGO::String luaName) {
         using Function = ReturnType(*)(ArgTypes...);

        // Create the function pointer in cpp
        lua_pushlightuserdata(lua, reinterpret_cast<void*>(function));

        // Push the function pointer to lua 
        lua_pushcclosure(lua, [](lua_State* L) {
            Function fn = reinterpret_cast<Function>(lua_touserdata(L, lua_upvalueindex(1)));
            return callLuaFunc(L, fn, std::index_sequence_for<ArgTypes...>{});
        }, 1);

        lua_setglobal(lua, luaName.c_str());
    }

    template<typename T>
    ErrorOrType<T, StringError> getFromFile(HUGO::String path) {
        // If path is invalid
       if (luaL_loadfile(lua, path.c_str()) != LUA_OK) {
            StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
            lua_pop(lua, 1);
            return ErrorOrType<T, StringError>(error);
        }

        // If lua crashes on runtime or returns nil
        if (lua_pcall(lua, 0, 1, 0) != LUA_OK) {
            StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
            lua_pop(lua, 1);
            return ErrorOrType<T, StringError>(error);
        }

        // Otherwise
        T result = lua_get<T>(lua, -1);
        lua_pop(lua, 1);
        return ErrorOrType<T, StringError>(result);
    }

    ErrorOrVoid runFile(HUGO::String path) {
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

        //Otherwise
        return ErrorOrVoid();
    }
};

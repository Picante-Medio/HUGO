#pragma once

#include "HugoTypes.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <luaconf.h>
#include <type_traits>
#include <utility>

template<typename T> T lua_get(lua_State* lua, int idx);

template<> inline int lua_get<int> (lua_State* lua, int idx) { return (int)luaL_checknumber(lua, idx); }
template<> inline float lua_get<float> (lua_State* lua, int idx) { return (float)luaL_checknumber(lua, idx); }
template<> inline double lua_get<double> (lua_State* lua, int idx) { return luaL_checknumber(lua, idx); }
template<> inline bool lua_get<bool> (lua_State* lua, int idx) { return lua_toboolean(lua, idx); }
template<> inline HUGO::String lua_get<HUGO::String>(lua_State* lua, int idx) { return luaL_checkstring(lua, idx); }

class LuaBinder {
    //pointer to the lua interpeter instance
    lua_State* lua;
    HUGO::String lastError;

    template<typename ReturnType, typename ...ArgTypes, std::size_t... I>
    static int callLuaFunc(lua_State* lua, ReturnType(*func)(ArgTypes...), std::index_sequence<I...>) {
        if(std::is_same<ReturnType, void>()) {
            func(lua_get<ArgTypes>(lua, I + 1)...);
            return 0; //Same as null
        }
        else {
            return lua_push_result(lua, fn(lua_get<ArgTypes>(lua, I + 1)...));
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
};

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
#include <vector>

//Functions to use the right lua types for arguments and return types
#pragma region HelperFunctions
//Auxiliar function to pop value from lua stack
template<typename T>
T lua_get(lua_State* lua, int idx) {
    if constexpr (std::is_same_v<T, HUGO::Int> || std::is_same_v<T, int>)
        return (HUGO::Int)luaL_checknumber(lua, idx);

    else if constexpr(std::is_same_v<T, HUGO::Float> || std::is_same_v<T, double> || std::is_same_v<T, float>)
        return (HUGO::Float)luaL_checknumber(lua, idx);

    else if constexpr (std::is_same_v<T, bool>)
        return lua_toboolean(lua, idx);

    else if constexpr (std::is_same_v<T, HUGO::String> || std::is_same_v<T, std::string>)
        return HUGO::String(luaL_checkstring(lua, idx));
    else
        static_assert(sizeof(T) == 0, "Lua: You should only declare functions with parameters of basic types that lua can read");
}

template<typename T>
//Auxiliar function to push value to lua stack
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
        throw "Lua: You should only declare functions with parameters of basic types that lua can read";
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

    // private functions that relates a given lua name to a function pointer call
    template<typename ReturnType, typename ...ArgTypes, std::size_t... I>
    static int callLuaFunc(lua_State* lua, ReturnType(*func)(ArgTypes...), std::index_sequence<I...>) {
        if constexpr (std::is_same_v<ReturnType, void> || std::is_same_v<ReturnType, ErrorOrVoid>) {
            func(lua_get<ArgTypes>(lua, I + 1)...);
            return 0; 
        }
        else {
            ReturnType result = func(lua_get<ArgTypes>(lua, I + 1)...);
            lua_push_result(lua, result);
            return 1;
        }
    }

    // private functions that relates a given lua name to an object method call
    template<typename ClassType, typename ReturnType, typename... ArgTypes, std::size_t... I>
    static int callLuaMethod(lua_State* lua, ClassType* obj, ReturnType(ClassType::*fn)(ArgTypes...), std::index_sequence<I...>) {
        if constexpr (std::is_same_v<ReturnType, void> || std::is_same_v<ReturnType, ErrorOrVoid>) {
            (obj->*fn)(lua_get<ArgTypes>(lua, I + 1)...);
            return 0;
        } else {
            ReturnType result = (obj->*fn)(lua_get<ArgTypes>(lua, I + 1)...);
            lua_push_result(lua, result);
            return 1;
        }
    }
public:
    static LuaBinder &Instance();
    int setLuaPath(HUGO::String path );
    ~LuaBinder();

    //Init must be called before using lua
    void Init();

    /* Register a standalone cpp function to call it from lua.
    Example for add function that returns int and receives two ints: 
    LuaBinder::Instance().registerFunc<int, int, int>(add, "add");
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

        // Create the function call with name in lua
        lua_setglobal(lua, luaName.c_str());
    }

    /* Register method of a cpp object to call it from lua.
    Example for add method in adder class that returns int and receives two ints: 
        LuaBinder::Instance().registerMethod<Adder, int, int, int>(*adderPtr, &Adder::add, "add");
    */
    template<typename ClassType, typename ReturnType, typename... ArgTypes>
    void registerMethod(ClassType& obj, ReturnType(ClassType::*fn)(ArgTypes...), HUGO::String name) {
        using Function = ReturnType(ClassType::*)(ArgTypes...);

        struct Method { ClassType* object; Function func; };

        Method* method = (Method*)lua_newuserdata(lua, sizeof(Method));
        method->object = &obj;
        method->func = fn;

        lua_pushcclosure(lua, [](lua_State* L) -> int {
            Method* method = (Method*)lua_touserdata(L, lua_upvalueindex(1));
            return callLuaMethod(L, method->object, method->func, std::index_sequence_for<ArgTypes...>{});
        }, 1);

        lua_setglobal(lua, name.c_str());
    }

    //Functions to call entire lua files
    
    // Call a lua script that returns nothing, or ignore its return value.
    // it returns an error if the file is not found or lua rises an error
    ErrorOrVoid runFile(HUGO::String path); 

    // Call a lua script and get its value as a T. If the value can't be converted to T.
    // it returns an error if the file is not found, lua rises an error or the value is not a T
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

    //Functions to call strings as lua code

    // Interpret a cpp string like its a standalone lua statement and ignore its return value if it exists.
    // it returns an error if the string cannot be interpreted as valid lua or if lua rises an error
    ErrorOrVoid interpretString(HUGO::String command);

    // Interpret a cpp string like its a standalone lua statement and get its return value as a T.
    // it returns an error if the string cannot be interpreted as valid lua, if lua rises an error or if the value is not a T
    template<typename T>
    ErrorOrType<T> getFromString(HUGO::String command) {
        //if string is invalid
        if (luaL_dostring(lua, command.c_str()) != LUA_OK) {
            StringError error = StringError(HUGO::String(lua_tostring(lua, -1)));
            lua_pop(lua, 1);
            return ErrorOrType<int>(error);
        }

        //Otherwise
        T result = lua_get<T>(lua, -1);
        lua_pop(lua, 1);
        return ErrorOrType<T, StringError>(result);
    }

    // Send a vector to lua as an ipair iterable table (table as an array).
    // The name of the lua table will can be optionally set, if not it will allways be "arrayTable"
    template<typename T>
    void sendTable(std::vector<T> array, HUGO::String luaGlobalName = "arrayTable") {
        // Create the "array"
        lua_newtable(lua);

        for (int i = 0; i < array.size(); i++) {
            // Push index
            lua_pushnumber(lua, i+1); // push the index

            // Push value
            lua_pushvalue(lua, array[i]);

            // Fix the table stack offset
            lua_settable(lua, -3);
        }

        lua_setglobal(lua, luaGlobalName.c_str());
    }
};

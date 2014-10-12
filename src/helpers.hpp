#pragma once

#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <exception>
#include "config.hpp"
#include "exceptions.hpp"
#include "misc.hpp"
#include <cstddef>
#include "macros.hpp"

namespace lua {
namespace LuaHelpers {

template<typename T> void luaPush(lua_State* state, T param);
template<> inline void luaPush<std::string>(lua_State* state, std::string param) { lua_pushstring(state, param.c_str()); }
template<> inline void luaPush<std::nullptr_t>(lua_State* state, std::nullptr_t) { lua_pushnil(state); }
LUA_PUSH_HELPER(lua_pushboolean, bool)
LUA_PUSH_HELPER(lua_pushinteger, int)
LUA_PUSH_HELPER(lua_pushinteger, long long)
LUA_PUSH_HELPER(lua_pushinteger, long)
LUA_PUSH_HELPER(lua_pushlightuserdata, void*)
LUA_PUSH_HELPER(lua_pushnumber, double)
LUA_PUSH_HELPER(lua_pushnumber, float)
LUA_PUSH_HELPER(lua_pushnumber, long double)
LUA_PUSH_HELPER(lua_pushstring, char const*)
LUA_PUSH_HELPER(lua_pushunsigned, unsigned int)
LUA_PUSH_HELPER(lua_pushunsigned, unsigned long long)
LUA_PUSH_HELPER(lua_pushunsigned, unsigned long)

template<typename T>
T luaGet(lua_State* state, int index);
LUA_GET_HELPER(lua_toboolean, lua_isboolean, bool)
LUA_GET_HELPER(lua_tointeger, lua_isnumber, int)
LUA_GET_HELPER(lua_tointeger, lua_isnumber, long long)
LUA_GET_HELPER(lua_tointeger, lua_isnumber, long)
LUA_GET_HELPER(lua_tonumber, lua_isnumber, double)
LUA_GET_HELPER(lua_tonumber, lua_isnumber, float)
LUA_GET_HELPER(lua_tonumber, lua_isnumber, long double)
LUA_GET_HELPER(lua_tostring, lua_isstring, const char*)
LUA_GET_HELPER(lua_tostring, lua_isstring, std::string)
LUA_GET_HELPER(lua_tounsigned, lua_isnumber, unsigned int)
LUA_GET_HELPER(lua_tounsigned, lua_isnumber, unsigned long long)
LUA_GET_HELPER(lua_tounsigned, lua_isnumber, unsigned long)
LUA_GET_HELPER(lua_touserdata, lua_islightuserdata, void*)

inline lua_State* pushArguments(lua_State* state) { return state; }

/**
 * @brief A helper to create lua_State* objects from arguments
 */
template<typename T>
inline lua_State* pushArguments(lua_State* state, T a) {
    LuaHelpers::luaPush<T>(state, a);
    return state;
}

/**
 * @brief A helper to create lua_State* objects from arguments
 */
template<typename T, typename... Args>
inline lua_State* pushArguments(lua_State* state, T a, Args... rest) {
    LuaHelpers::luaPush<T>(state, a);
    pushArguments(state, rest...);
    return state;
}

template<std::size_t Index, typename T>
inline typename std::enable_if<Index >= std::tuple_size<T>::value, void>::type
forEach(lua_State*, T&) {}

template<std::size_t Index = 0, typename T>
inline typename std::enable_if<Index < std::tuple_size<T>::value, void>::type
forEach(lua_State* lua, T& t) {
    std::get<Index>(t) = luaGet<typename std::tuple_element<Index, T>::type>
        (lua, 0 - static_cast<int>(std::tuple_size<T>::value) + static_cast<int>(Index));
    if (Index < std::tuple_size<T>::value) {
        forEach<Index + 1>(lua, t);
    }
}

inline std::size_t getToStack(lua_State* lua, const std::string& var) {
    auto splits = misc::split(var, '.');
    std::size_t pushes = 0;

    auto err = [&](std::size_t index) -> bool {
        auto err = lua_isnil(lua, -1) || lua_isnone(lua, -1);
        if (err) {
            std::string joined = splits[0];
            for (std::size_t i = 1; i < index; ++i) { joined += '.' + splits[i]; }
            throw exceptions::NoSuchKey("Error! No such field: " + joined);
        }
        pushes += 1;
        return err;
    };

    lua_getglobal(lua, splits[0].c_str());
    if (err(0)) { return pushes; }

    for (std::size_t i = 1; i < splits.size(); ++i) {
        lua_getfield(lua, -1, splits[i].c_str());
        if (err(i)) { return pushes; }
    }
    return pushes;
}

inline void popStack(lua_State* lua, int amount) {
    lua_pop(lua, amount);
}

} // namespace LuaHelpers
} // namespace lua

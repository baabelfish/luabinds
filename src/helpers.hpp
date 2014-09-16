#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <exception>
#include "config.hpp"
#include "exceptions.hpp"
#include "misc.hpp"

namespace lua {
namespace LuaHelpers {

template<typename T> void luaPush(lua_State* state, T param);
template<> inline void luaPush<char const*>(lua_State* state, char const* param) { lua_pushstring(state, param); }
template<> inline void luaPush<std::string>(lua_State* state, std::string param) { lua_pushstring(state, param.c_str()); }
template<> inline void luaPush<int>(lua_State* state, int param) { lua_pushinteger(state, param); }
template<> inline void luaPush<float>(lua_State* state, float param) { lua_pushnumber(state, param); }
template<> inline void luaPush<bool>(lua_State* state, bool param) { lua_pushboolean(state, param); }

template<typename T>
T luaGet(lua_State* state, int index);

template<> inline bool luaGet<bool>(lua_State* state, int index) {
    if (!lua_isboolean(state, index)) { throw lua::exceptions::NotValidType("Given type is not a boolean"); }
    return lua_toboolean(state, index);
};

template<> inline int luaGet<int>(lua_State* state, int index) {
    if (!lua_isnumber(state, index)) { throw lua::exceptions::NotValidType("Given type is not a number"); }
    return lua_tointeger(state, index);
};

template<> inline std::string luaGet<std::string>(lua_State* state, int index) {
    if (!lua_isstring(state, index)) { throw lua::exceptions::NotValidType("Given type is not a string"); }
    return lua_tostring(state, index);
};

template<> inline float luaGet<float>(lua_State* state, int index) {
    if (!lua_isnumber(state, index)) { throw lua::exceptions::NotValidType("Given type is not a number"); }
    return lua_tonumber(state, index);
};

template<> inline double luaGet<double>(lua_State* state, int index) {
    if (!lua_isnumber(state, index)) { throw lua::exceptions::NotValidType("Given type is not a number"); }
    return lua_tonumber(state, index);
};

template<> inline char const* luaGet<char const*>(lua_State* state, int index) {
    if (!lua_isstring(state, index)) { throw lua::exceptions::NotValidType("Given type is not a string"); }
    return lua_tostring(state, index);
};


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

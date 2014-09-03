#pragma once

#include "helpers.hpp"
#include "misc.hpp"

namespace lua {

class State {
    lua_State* m_state;
    mutable int m_size;

public:
    State(lua_State* state):
        m_state(state),
        m_size(0) {}

    /**
     * @brief Gets the nth-argument and retrieves it with the templated type
     *
     * @param index index of the argument, -1 is return value and 1 the first param
     *
     * @return
     */
    template<typename T>
    T get(int index) {
        return LuaHelpers::luaGet<T>(m_state, index);
    };
    std::string get(int index) {
        return LuaHelpers::luaGet<std::string>(m_state, index);
    };

    /**
     * @brief Pushes n-amount of arguments for the function to return to Lua
     */
    template<typename... Args>
    void push(Args... args) {
        m_size += sizeof...(args);
        LuaHelpers::pushArguments(m_state, args...);
    };

    /**
     * @brief Returns the number of parameters the function was given
     */
    int sizeParameters() const { return lua_gettop(m_state); }
    bool hasParameters() const { return lua_gettop(m_state) > 0; }

    /**
     * @brief Returns the amount of return values the function returns
     */
    bool hasRevals() const { return m_size > 0; }
    int sizeRevals() const { return m_size; }
};

} // namespace lua

#pragma once

#include "helpers.hpp"
#include "misc.hpp"

namespace lua {

class State {
    lua_State* m_state;
    mutable int m_psize;
    mutable int m_rsize;

public:
    State(lua_State* state):
        m_state(state),
        m_psize(lua_gettop(state)),
        m_rsize(0) {}

    /**
     * @brief Gets the nth-argument and retrieves it with the templated type
     *
     * @param index index of the argument, -1 is return value and 1 the first param
     *
     * @return
     */
    template<typename T>
    T get(int index) const {
        return LuaHelpers::luaGet<T>(m_state, index);
    };
    std::string get(int index) const {
        return LuaHelpers::luaGet<std::string>(m_state, index);
    };

    template<typename T>
    inline T operator[](int index) const { return get<T>(index); }
    inline std::string operator[](int index) const { return get<std::string>(index); }

    bool isNil(int index) const {
        return lua_isnil(m_state, index);
    };


    /**
     * @brief Pushes n-amount of arguments for the function to return to Lua
     */
    template<typename... Args>
    void push(Args... args) {
        m_rsize += sizeof...(args);
        LuaHelpers::pushArguments(m_state, args...);
    };

    /**
     * @brief Returns the number of parameters the function was given
     */
    int sizeParameters() const { return m_psize; }
    bool hasParameters() const { return m_psize > 0; }

    /**
     * @brief Returns the amount of return values the function returns
     */
    bool hasRevals() const { return m_rsize > 0; }
    int sizeRevals() const { return m_rsize; }
};

} // namespace lua

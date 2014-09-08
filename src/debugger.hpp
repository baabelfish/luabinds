#pragma once

#include "config.hpp"
#include "lua.hpp"

namespace lua {

class Debugger {
public:
    Debugger(const lua::Lua& lua):
        m_lua(lua.handle()) {
    }

    virtual ~Debugger() {
    }

    bool getStack(int level) {
        lua_Debug* ar = nullptr;
        auto re = lua_getstack(m_lua, level, ar);
        return re;
    }


private:
    lua_State* m_lua;
};

} // namespace lua

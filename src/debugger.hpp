#pragma once

#include "config.hpp"
#include "lua.hpp"
#include "helpers.hpp"

namespace lua {

class Debugger {
public:
    Debugger(lua_State* handle):
        m_lua(handle) {
    }

    Debugger(const lua::Lua& lua):
        m_lua(lua.handle()) {
    }

    virtual ~Debugger() {
    }

    void info(std::string obj) {
        // lua_Debug ar;
        // lua_getglobal(m_lua, obj.c_str());
        // LuaHelpers::getToStack(m_lua, obj);

        // if (lua_getinfo(m_lua, "n", &ar)) {
        //     std::cout << ar.name << std::endl;
        //     std::cout << ar.namewhat << std::endl;
        // }
        // if (lua_getinfo(m_lua, "S", &ar)) {
        //     std::cout << ar.source << std::endl;
        //     std::cout << ar.short_src << std::endl;
        //     std::cout << ar.linedefined << std::endl;
        //     std::cout << ar.lastlinedefined << std::endl;
        //     std::cout << ar.what << std::endl;
        // }
        // if (lua_getinfo(m_lua, "l", &ar)) {
        //     std::cout << ar.currentline << std::endl;
        // }
        // if (lua_getinfo(m_lua, "t", &ar)) {
        //     std::cout << ar.istailcall << std::endl;
        // }
        // if (lua_getinfo(m_lua, "u", &ar)) {
        //     std::cout << ar.nups << std::endl;
        //     std::cout << ar.nparams << std::endl;
        //     std::cout << ar.isvararg << std::endl;
        // }
    }


private:
    lua_State* m_lua;
};

} // namespace lua

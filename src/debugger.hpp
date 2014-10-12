#pragma once

#include "config.hpp"
#include "lua.hpp"
#include "helpers.hpp"

#include <iomanip>
#include <unordered_map>
#include <vector>
#include <string>

namespace lua {

class Debugger {
public:
    struct StackInfo {
        int line;
        int func_firstline;
        int func_lastline;
        int event;
        unsigned param_amount;
        unsigned upvalues;
        bool is_tailcall;
        bool is_vararg;
        std::string what;
        std::string source;
        std::string short_source;
        std::string name;
        std::string namewhat;

        StackInfo(lua_Debug* ar):
            line(ar->currentline),
            func_firstline(ar->linedefined),
            func_lastline(ar->lastlinedefined),
            event(ar->event),
            param_amount(ar->nparams),
            upvalues(ar->nups),
            is_tailcall(ar->istailcall),
            is_vararg(ar->isvararg),
            what(ar->what),
            source(ar->source),
            short_source(ar->short_src),
            name(ar->name ? ar->name : ""),
            namewhat(ar->namewhat)
        {}
    };

    static auto& callbacks() {
        static std::unordered_map<lua_State*, std::function<bool(StackInfo)>> s_tracebacks;
        return s_tracebacks;
    }

    static void hook(lua_State* state, lua_Debug* ar) {
        lua_getinfo(state, "nSlLtuf", ar);
        auto it = callbacks().find(state);
        if (it != callbacks().end()) {
            it->second(StackInfo(ar));
        }
    }

    template<typename F>
    static void attach(const lua::Lua& lua, F f) { attach(lua.handle(), std::forward<F>(f)); }

    template<typename F>
    static void attach(lua_State* handle, F f) {
        callbacks()[handle] = std::move(f);
        lua_sethook(handle, hook, LUA_MASKCALL | LUA_MASKRET | LUA_MASKLINE, 0);
    }

    static void detach(const lua::Lua& lua) { detach(lua.handle()); }
    static void detach(lua_State* handle) {
        lua_sethook(handle, hook, 0, 0);
        callbacks().erase(handle);
    }

};

} // namespace lua

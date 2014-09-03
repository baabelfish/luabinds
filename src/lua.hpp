#pragma once

#include <lua.hpp>
#include <iostream>
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include "misc.hpp"
#include "helpers.hpp"
#include "state.hpp"

namespace lua {

class Lua {
public:
    /**
     * @brief Creates a lua context by running a file
     *
     * @param file Name of the file to run
     * @param api Variadic arguments for apifunctions seen by the script.
     * Given in pairs (std::string, std::function<void(lua::State&)>) where
     * first one is the function identifier and second is the function to call.
     */
    template<typename... Api>
    Lua(const std::string& file, Api... api):
        m_file(file),
        m_lua(luaL_newstate())
    {
        if (!m_lua) {
            throw std::bad_alloc();
        }
        else {
            attach(std::forward<Api>(api)...);
            luaL_openlibs(m_lua);
            luaL_dofile(m_lua, file.c_str());
        }
    }

    virtual ~Lua() {
        lua_close(m_lua);
    }

    /**
     * @brief Specialization for get-without template params.
     *
     * @param var
     * @param otherwise
     *
     * @return
     */
    inline std::string get(std::string var, std::string&& otherwise = "") {
        return get<std::string>(var, std::move(otherwise));
    }

    /**
      * @brief Get's a value set by the script
      *
      * @param var Value of the script. Nested keys seperated by '.' (e.g. "toplevel.some.value")
      * @param otherwise If not found, replace it by this.
      *
      * @return Returns the found attribute in type T
      */
    template<typename T>
    T get(std::string var, T&& otherwise = T()) {
        try {
            auto popped = getToStack(m_lua, var);
            otherwise = LuaHelpers::luaGet<T>(m_lua, -1);
            lua_pop(m_lua, popped);
        }
        catch (NoSuchKey ex) {
            std::cout << ex.what() << std::endl;
            throw ex;
        }
        return otherwise;
    }

    /**
     * @brief Finds a function by id. Template parameters are types for return values.
     *
     * @param func Name of the function to return
     *
     * @return Returns a function closure that can be called with variadic parameters
     * e.g.:
     *   lua.call("hello")();
     *   lua.call<int>("sum")(1,2,3) // result: 6 (type: integer);
     *   lua.call<int, int>("twoRandom")() // result: {42, 7} (type: std::tuple<int, int>);
     */
    template<typename... Reval>
    auto call(std::string func) {
        return createCall<Reval...>(func);
    }

private:
    std::string m_file;
    lua_State* m_lua;

    template<typename... Args>
    void attach() {}

    template<typename... Args>
    void attach(std::string id, std::function<void(State&)> f, Args... rest) {
        // Ugly trick to get scope variables in and I got no idea why it works...
        // FIXME: Seek alternatives
        static std::function<void(State&)> func;
        func = f;
        auto lf = [](lua_State* state) -> int {
            State params(state);
            func(params);
            return params.sizeRevals();
        };
        lua_register(m_lua, id.c_str(), lf);
        attach(rest...);
    }

    // Clang fails:
    // typename std::enable_if<sizeof...(Reval) > 1>::type* = nullptr>
    template<typename... Reval,
             typename std::enable_if<sizeof...(Reval) >= 2>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            std::tuple<Reval...> res;
            lua_getglobal(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lua_call(lua, sizeof...(args), sizeof...(Reval));
            LuaHelpers::forEach(lua, res);
            return res;
        };
    }

    template<typename... Reval,
             typename std::enable_if<sizeof...(Reval) == 1>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            std::tuple<Reval...> res;
            lua_getglobal(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lua_call(lua, sizeof...(args), sizeof...(Reval));
            return LuaHelpers::luaGet<typename std::tuple_element<0, decltype(res)>::type>(lua, -1);
        };
    }

    template<typename... Reval,
             typename std::enable_if<sizeof...(Reval) == 0>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            lua_getglobal(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lua_call(lua, sizeof...(args), sizeof...(Reval));
        };
    }

    static std::size_t getToStack(lua_State* lua, const std::string& var) {
        auto splits = misc::split(var, '.');
        std::size_t pushes = 0;

        auto err = [&](std::size_t index) -> bool {
            auto err = lua_isnil(lua, -1);
            if (err) {
                std::string joined = splits[0];
                for (std::size_t i = 1; i < index; ++i) {
                    joined += '.' + splits[i];
                }
                throw NoSuchKey("Error! No such field: " + joined);
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
};

} // namespace lua

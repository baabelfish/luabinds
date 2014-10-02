#pragma once

#include <iostream>
#include <map>
#include <tuple>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include <utility>

#include "config.hpp"
#include "misc.hpp"
#include "helpers.hpp"
#include "state.hpp"
#include "exceptions.hpp"
#include "constants.hpp"
#include "function_traits.hpp"

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
    Lua(Api... api):
        m_lua(luaL_newstate()) {
        static_assert(sizeof...(api) % 2 == 0, "Api should be given in (string, function) -pairs");

        if (!m_lua) { throw std::bad_alloc(); }
        luaL_openlibs(m_lua);
        attachApi(std::forward<Api>(api)...);
    }

    virtual ~Lua() {
        lua_close(m_lua);
    }

    static Lua defAndRun(std::string file) {
        Lua lua;
        lua.eval(file);
        return lua;
    }

    template<typename... Api>
    static Lua defAndRun(std::string file, Api... api) {
        Lua lua(std::forward<Api>(api)...);
        lua.eval(file);
        return lua;
    }

    void evalString(std::string str) {
        auto err = luaL_dofile(m_lua, str.c_str());
        if (err) {
            throw exceptions::CouldNotParse("Could not parse");
        }
    }

    void eval(std::string file) {
        auto err = luaL_dofile(m_lua, file.c_str());
        if (err) {
            throw exceptions::CouldNotParse("Lua script not found or erroneous: " + file);
        }
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
        auto ssize = LuaHelpers::getToStack(m_lua, var);
        otherwise = LuaHelpers::luaGet<T>(m_lua, -1);
        LuaHelpers::popStack(m_lua, ssize);
        return otherwise;
    }

    inline std::string get(std::string var, std::string&& otherwise = "") {
        return get<std::string>(var, std::move(otherwise));
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
        return createCall<internal::FunctionType::Normal, Reval...>(func);
    }

    template<typename... Reval>
    auto call(lua_CFunction func) {
        return [=](auto... args) {
            lua_pushcfunction(m_lua, func);
            // LuaHelpers::pushArguments(m_lua, std::forward<decltype(args)>(args)...);
            lcall<internal::FunctionType::Normal, sizeof...(args), sizeof...(Reval)>(m_lua);
            lua_settop(m_lua, 0);
        };
    }

    template<typename... Reval>
    auto pcall(std::string func) {
        return createCall<internal::FunctionType::Safe, Reval...>(func);
    }

    template<typename... Reval>
    auto fcall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            LuaHelpers::getToStack(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lcall<internal::FunctionType::Function, sizeof...(args), sizeof...(Reval)>(lua);
            // if (!lua_iscfunction(lua, -1)) {
            //     throw exceptions::NotValidType("Given type is not a function");
            // }
            auto lf = lua_tocfunction(lua, -1);
            lua_settop(lua, 0);
            return lf;
        };
    }

    /**
     * @brief Returns the lua version
     */
    int version() const {
        return static_cast<int>(*lua_version(m_lua));
    }

    /**
     * @brief Checks if the stack has extra amount of slots left
     *
     * @param extra Amount of required slots
     *
     * @return false if there is not enough slots
     */
    bool checkstack(int extra) const {
        return lua_checkstack(m_lua, extra);
    }

    bool garbageCollector(lua::GC task) {
        return lua_gc(m_lua, static_cast<int>(task), 0);
    }

    lua_State* handle() const {
        return m_lua;
    }

    template<typename F>
    auto& attachWithState(std::string name, F f) {
        static F temp = std::move(f);
        temp = std::move(f);
        auto cl = [](lua_State* state) -> int {
            State params(state);
            temp(params);
            return params.sizeRevals();
        };
        lua_register(m_lua, name.c_str(), cl);
        return *this;
    }

    template<typename F>
    auto& attach(std::string id, F f) {
        using T = tmp::function_traits<F>;
        typename T::param_types x;
        reg<typename T::return_type>(id, f, x);
        return *this;
    }


private:
    lua_State* m_lua;

    template<typename... Args>
    void attachApi() {}

    template<typename... Args>
    void attachApi(std::string id, std::function<void(State&)> f, Args... rest) {
        static std::function<void(State&)> func;
        func = f;
        auto lf = [](lua_State* state) -> int {
            State params(state);
            func(params);
            return params.sizeRevals();
        };
        lua_register(m_lua, id.c_str(), lf);
        attachApi(std::forward<Args>(rest)...);
    }

    // New function api
    // ================================================================================
    template<std::size_t Index, std::size_t End, typename T, typename std::enable_if<Index == End>::type* = nullptr>
    static inline auto collect(T t, lua_State*) { return std::move(t); }

    template<std::size_t Index, std::size_t End, typename T, typename std::enable_if<Index < End>::type* = nullptr>
    static inline auto collect(T t, lua_State* state) {
        using TE = typename std::tuple_element<Index, T>::type;
        std::get<Index>(t) = LuaHelpers::luaGet<TE>(state, Index + 1);
        return std::move(collect<Index + 1, End, T>(std::move(t), state));
    }

    template<typename... Args>
    static inline auto collectAll(lua_State* state) {
        return collect<0, sizeof...(Args), std::tuple<Args...>>(std::tuple<Args...>(), state);
    }

    template<typename R, typename... Args, typename F,
             typename std::enable_if<std::is_void<R>::value>::type* = nullptr>
    inline void reg(std::string id, F f) {
        static std::function<R(Args...)> func;
        func = f;
        auto lf = [](lua_State* state) -> int {
            auto t = collectAll<Args...>(state);
            misc::call(func, t);
            return 0;
        };
        lua_register(m_lua, id.c_str(), lf);
    }

    template<typename R, typename... Args, typename F,
             typename std::enable_if<!std::is_void<R>::value>::type* = nullptr>
    inline void reg(std::string id, F f) {
        static std::function<R(Args...)> func;
        func = f;
        auto lf = [](lua_State* state) -> int {
            auto t = collectAll<Args...>(state);
            LuaHelpers::luaPush(state, misc::call(func, t));
            return 1;
        };
        lua_register(m_lua, id.c_str(), lf);
    }

    template<typename R, typename F, template<typename...> class P, typename... Args>
    void reg(std::string id, F f, const P<Args...>&) {
        reg<R, Args...>(id, f);
    }
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // createCall
    // ================================================================================
    // Clang fails:
    // typename std::enable_if<sizeof...(Reval) > 1>::type* = nullptr>
    template<internal::FunctionType UseSafe, typename... Reval,
             typename std::enable_if<sizeof...(Reval) >= 2>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            std::tuple<Reval...> res;
            LuaHelpers::getToStack(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lcall<UseSafe, sizeof...(args), sizeof...(Reval)>(lua);
            LuaHelpers::forEach(lua, res);
            lua_settop(lua, 0);
            return res;
        };
    }

    template<internal::FunctionType UseSafe, typename... Reval,
             typename std::enable_if<sizeof...(Reval) == 1>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            LuaHelpers::getToStack(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lcall<UseSafe, sizeof...(args), sizeof...(Reval)>(lua);
            auto res = LuaHelpers::luaGet<typename std::tuple_element<0, std::tuple<Reval...>>::type>(lua, -1);
            lua_settop(lua, 0);
            return res;
        };
    }

    template<internal::FunctionType UseSafe, typename... Reval,
             typename std::enable_if<sizeof...(Reval) == 0>::type* = nullptr>
    auto createCall(std::string func) {
        return [lua = m_lua, f = func](auto... args) {
            LuaHelpers::getToStack(lua, f.c_str());
            LuaHelpers::pushArguments(lua, std::forward<decltype(args)>(args)...);
            lcall<UseSafe, sizeof...(args), sizeof...(Reval)>(lua);
            lua_settop(lua, 0);
        };
    }
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // lua_call
    // ================================================================================
    template<internal::FunctionType CallType, std::size_t ArgSize, std::size_t RevalSize,
             typename std::enable_if<CallType == internal::FunctionType::Function>::type* = nullptr>
    static void lcall(lua_State* lua) {
        lua_call(lua, ArgSize, RevalSize);
    }

    template<internal::FunctionType CallType, std::size_t ArgSize, std::size_t RevalSize,
             typename std::enable_if<CallType == internal::FunctionType::Normal>::type* = nullptr>
    static void lcall(lua_State* lua) {
        lua_call(lua, ArgSize, RevalSize);
    }

    template<internal::FunctionType CallType, std::size_t ArgSize, std::size_t RevalSize,
             typename std::enable_if<CallType == internal::FunctionType::Safe>::type* = nullptr>
    static void lcall(lua_State* lua) {
        int as = ArgSize;
        int rs = RevalSize;
        int third = 0;

        switch (lua_pcall(lua, as, rs, third)) {
            case LUA_ERRRUN: throw exceptions::CallFailed("Runtime error occurred");
            case LUA_ERRMEM: throw exceptions::CallFailed("Memory allocation error");
            case LUA_ERRERR: throw exceptions::CallFailed("Message handler failed");
            case LUA_ERRGCMM: throw exceptions::CallFailed("Garbage collector failed");
            case LUA_OK: return;
            default: return;
        };
    }
    // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

};

} // namespace lua

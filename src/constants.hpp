#pragma once

#include "config.hpp"

namespace lua {
enum class GC {
    Collect = LUA_GCCOLLECT,
    Count = LUA_GCCOUNT,
    Countb = LUA_GCCOUNTB,
    Gen = LUA_GCGEN,
    Inc = LUA_GCINC,
    Isrunning = LUA_GCISRUNNING,
    Restart = LUA_GCRESTART,
    Setpause = LUA_GCSETPAUSE,
    Setstepmul = LUA_GCSETSTEPMUL,
    Step = LUA_GCSTEP,
    Stop = LUA_GCSTOP
};

namespace internal {
enum class FunctionType {
    Normal,
    Safe,
    Function
};
} // namespace internal

} // namespace lua

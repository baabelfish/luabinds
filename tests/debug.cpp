#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"

static const std::string Testfile = "tests/lua/luatest.lua";
static const std::string Context1 = "tests/lua/context1.lua";
static const std::string Context2 = "tests/lua/context2.lua";
static const std::string Functions = "tests/lua/functions.lua";
static const std::string Debug = "tests/lua/debug.lua";

yTestPackage debug([]{
    describe("luabinds debug utility", []{
        it("", []{
            lua::Lua lua;
            lua::Debugger::attach(lua, [&](lua::Debugger::StackInfo) { return false; });
            lua.eval(Debug);
            lua::Debugger::detach(lua);
        });
    });
});

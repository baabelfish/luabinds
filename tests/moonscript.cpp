#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <tuple>

static const std::string Path = "tests/moonscript/";

yTestPackage moonscript([]{
    describe("luabinds", []{

        it("", []{
            lua::Lua lua(Path + "moonscript.lua");
            auto re = lua.call<int>("MultiplyAndSum")(2, 3);
            Assert().isEqual(re, 11);
        });

        it("", []{
            lua::Lua lua(Path + "moonscript2.lua");
        });

    });
});

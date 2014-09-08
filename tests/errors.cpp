#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <iostream>
#include <chrono>

static const std::string Testfile = "tests/lua/luatest.lua";

yTestPackage errors([]{
    describe("error handling", []{

        it("throws an exception when file can't be found", []{
            bool works = false;
            try { lua::Lua lua("i dont exist :("); }
            catch (lua::exceptions::CouldNotParse& e) { works = true; }
            Assert().isTrue(works);
        });

        it("throws an exception on erroneous file", []{
            bool works = false;
            try { lua::Lua lua("lua/wontexecute.lua"); }
            catch (lua::exceptions::CouldNotParse& e) { works = true; }
            Assert().isTrue(works);
        });

        it("throws an exception when accessing a non existant variable", []{
            bool works = false;
            lua::Lua lua(Testfile);
            try { auto val = lua.get("idont.exist"); }
            catch (lua::exceptions::NoSuchKey& e) { works = true; }
            Assert().isTrue(works);
        });

    });
});

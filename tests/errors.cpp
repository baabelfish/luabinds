#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <iostream>
#include <chrono>

static const std::string Testfile = "tests/lua/luatest.lua";

yTestPackage errors([]{
    describe("error handling", []{

        it("throws an exception when file can't be found", []{
            bool works = false;
            try {
                lua::Lua lua;
                lua.eval("i dont exist :(");
            }
            catch (lua::exceptions::CouldNotParse& e) { works = true; }
            Assert().isTrue(works);
        });

        it("throws an exception on erroneous file", []{
            bool works = false;
            try {
                lua::Lua lua;
                lua.eval("lua/wontexecute.lua");
            }
            catch (lua::exceptions::CouldNotParse& e) { works = true; }
            Assert().isTrue(works);
        });

        it("throws an exception when accessing a non existing variable", []{
            bool works = false;
            lua::Lua lua;
            lua.eval(Testfile);
            try { lua.get("idont.exist"); }
            catch (lua::exceptions::NoSuchKey& e) { works = true; }
            Assert().isTrue(works);
        });

        it("throws an exception when conversion fails", []{
            bool works = false;
            lua::Lua lua;
            lua.eval(Testfile);
            try { lua.get<int>("player.name"); }
            catch (lua::exceptions::NotValidType& e) { works = true; }
            Assert().isTrue(works);
        });

    });
});

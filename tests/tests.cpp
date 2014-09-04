#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <tuple>

static const std::string Testfile = "tests/lua/luatest.lua";

yTestPackage pkg([]{
    describe("luabinds", []{

        it("can execute a script", []{
            lua::Lua lua(Testfile);
        });

        it("can fetch globals", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.get("global"), "string");
        });

        it("works with lua types", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.get<std::string>("data_str"), "a string variable")
                    .isEqual(lua.get<int>("data_integer"), 10)
                    .isEqual(lua.get<bool>("data_boolean"), true)
                    .isEqual(lua.get<float>("data_float"), 7.62);
        });

        it("works with tables", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.get<int>("player.health"), 100)
                    .isEqual(lua.get<int>("player.mana"), 25)
                    .isEqual(lua.get<int>("player.stamina"), 125);
        });

        it("works with nested tables", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.get<int>("player.minion.health"), 10);
        });

        it("works with void functions", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.get<int>("variable"), 0);
            lua.call("noreval")();
            Assert().isEqual(lua.get<int>("variable"), 1);
        });

        it("works with single reval functions", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.call<std::string>("ping")(), "pong");
        });

        it("works with multi reval functions", []{
            lua::Lua lua(Testfile);
            auto res = lua.call<int, std::string>("multireval")();
            Assert().isEqual(std::get<0>(res), 1)
                    .isEqual(std::get<1>(res), "hello");
        });

        it("works with variadic functions", []{
            lua::Lua lua(Testfile);
            auto res = lua.call<int, std::string>("multireval")();
            Assert().isEqual(lua.call<int>("sum")(1), 1)
                    .isEqual(lua.call<int>("sum")(1, 2), 3)
                    .isEqual(lua.call<int>("sum")(1, 2, 3), 6)
                    .isEqual(lua.call<int>("sum")(1, 2, 3, 4), 10);
        });

        it("can attach functions for use from lua", []{
            bool hasVisitedHello = false;
            bool hasVisitedFirst = false;
            bool hasVisitedOther = false;

            lua::Lua lua(Testfile,
                "first", [&](lua::State& s) {
                    hasVisitedFirst = true;
                    s.push(s.get(0));
                },
                "helloFromCpp", [&](lua::State& s) {
                    hasVisitedHello = true;
                    s.push(s.sizeParameters() > 0 ? "Hello, " + s.get(1) : "Hello!");
                },
                "other", [&](lua::State& s) {
                    hasVisitedOther = true;
                    s.push("Other func");
                });

            Assert().isTrue(hasVisitedHello)
                    .isTrue(hasVisitedOther)
                    .isFalse(hasVisitedFirst);
        });

        it("works with function variables", []{
            lua::Lua lua(Testfile);
            Assert().isEqual(lua.call<int>("somefunc")(), 17);
        });

        it("works with variadic cpp-functions", []{
            lua::Lua lua(Testfile,
                "cppsum", [](lua::State& s) {
                    int sum = 0;
                    for (int i = 1; i < s.sizeParameters() + 1; ++i) {
                        sum += s.get<int>(i);
                    }
                    s.push(sum);
                });
            Assert().isEqual(lua.get<int>("cppsumValue"), 15);
        });

        it("can return an function as an object", []{
            lua::Lua lua(Testfile);
            auto f = lua.call<int>("sum");
            Assert().isEqual(f(1, 2), 3)
                    .isEqual(f(1, "2", 3), 6)
                    .isEqual(f(1, 10, 100), 111);
        });

    });
});

yTestExec();

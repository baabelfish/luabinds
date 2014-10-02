#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <iostream>

static const std::string Testfile = "tests/lua/containers.lua";

yTestPackage containers([]{
    describe("container handling", []{
        std::vector<int> data{1,2,3};
        using data_value_type = decltype(data)::value_type;

        lua::Lua lua;
        lua.attachWithState("v_size", [data = &data](lua::State& s) { s.push(data->size()); });
        lua.attachWithState("v_clear", [data = &data](lua::State&) { data->clear(); });
        lua.attachWithState("v_empty", [data = &data](lua::State& s) { s.push(data->empty()); });
        lua.attachWithState("v_at", [data = &data](lua::State& s) {
            for (int i = 1; i <= s.sizeParameters(); ++i) {
                s.push(data->at(s.get<std::size_t>(i)));
            }
        });
        lua.attachWithState("v_set", [data = &data](lua::State& s) {
            data->at(s.get<int>(1)) = s.get<data_value_type>(2);
        });
        lua.attachWithState("v_push", [data = &data](lua::State& s) {
            data->push_back(s.get<data_value_type>(1));
        });

        lua.eval(Testfile);

        it("it can cope with containers", [&]{
            Assert().isFalse(lua.get<bool>("fail"));
        });
    });
});

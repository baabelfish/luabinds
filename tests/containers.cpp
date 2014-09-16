#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include <iostream>

static const std::string Testfile = "tests/lua/containers.lua";

yTestPackage containers([]{
    describe("container handling", []{
        std::vector<int> data{1,2,3};
        using data_value_type = decltype(data)::value_type;

        lua::Lua lua;
        lua.attach("v_size", [data = &data](lua::State& s) { s.push(data->size()); });
        lua.attach("v_clear", [data = &data](lua::State&) { data->clear(); });
        lua.attach("v_empty", [data = &data](lua::State& s) { s.push(data->empty()); });
        lua.attach("v_at", [data = &data](lua::State& s) {
            for (int i = 1; i <= s.sizeRevals(); ++i) {
                s.push(data->at(s.get<std::size_t>(i)));
            }
        });
        lua.attach("v_set", [data = &data](lua::State& s) {
            data->at(s.get<int>(1)) = s.get<data_value_type>(2);
        });
        it("it can cope with containers", []{

        });
    });
});

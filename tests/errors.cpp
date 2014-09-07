#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"

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

    });
});

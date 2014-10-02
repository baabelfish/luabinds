#include "../lib/ytest/ytest.hpp"
#include "../luabinds.hpp"
#include "../src/function_traits.hpp"
#include <tuple>

int freeFunction(double, std::string) { return 0; }

struct Functor {
    int operator()(double a, std::string) { return a; }
};

auto stdfunc = [](double, std::string) { return 0; };

template<typename X>
void test() {
    using T = tmp::function_traits<X>;
    static_assert(std::is_same<typename T::return_type, int>::value, "Return value should be of type int");
    static_assert(std::tuple_size<typename T::param_types>::value == 2, "Tuple parameter amount must be 2");
    static_assert(std::is_same<typename std::tuple_element<0, typename T::param_types>::type, double>::value, "First parameter type should be int");
    static_assert(std::is_same<typename std::tuple_element<1, typename T::param_types>::type, std::string>::value, "Second parameter type should be std::string");
}

yTestPackage ptmp([]{
    describe("TMP Utilities", []{
        it("Works with free functions", []{ test<decltype(freeFunction)>(); });
        it("Works with std::function", []{ test<decltype(stdfunc)>(); });
    });
});

#pragma once

#include <vector>
#include <string>
#include <utility>
#include <tuple>

namespace lua {
namespace misc {

// Clang 3.4 fails with std::index_sequence*
template<std::size_t... Ints> using index_sequence = std::integer_sequence<std::size_t, Ints...>;
template<class... T> using index_sequence_for = std::make_index_sequence<sizeof...(T)>;

inline std::vector<std::string> split(const std::string& var, char ch) {
    std::vector<std::string> splits;
    std::size_t last = 0;
    for (std::size_t i = 0; i < var.size(); ++i) {
        if (var[i] == ch) {
            splits.push_back(var.substr(last, i - last));
            last = i + 1;
        }
    }
    splits.push_back(var.substr(last));
    return splits;
}

// http://stackoverflow.com/a/20441189
template<typename F, template<typename...> class Params, typename... Args, std::size_t... I>
auto call_helper(const F f, const Params<Args...>& params, index_sequence<I...>) {
    return f(std::get<I>(params)...);
}
template<typename F, template<typename...> class Params, typename... Args>
auto call(const F f, const Params<Args...>& params) {
    return call_helper(f, params, index_sequence_for<Args...>{});
}


} // namespace misc
} // namespace lua

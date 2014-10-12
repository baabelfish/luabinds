#pragma once

#include <vector>
#include <string>
#include <utility>
#include <tuple>

namespace lua {
namespace misc {

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
auto call_helper(const F f, const Params<Args...>& params, std::index_sequence<I...>) {
    return f(std::get<I>(params)...);
}
template<typename F, template<typename...> class Params, typename... Args>
auto call(const F f, const Params<Args...>& params) {
    return call_helper(f, params, std::index_sequence_for<Args...>{});
}


} // namespace misc
} // namespace lua

#pragma once

#include <vector>
#include <string>

namespace lua {
namespace misc {

std::vector<std::string> split(const std::string& var, char ch) {
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


} // namespace misc
} // namespace lua

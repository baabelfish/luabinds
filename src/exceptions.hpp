#pragma once

#include <string>
#include <stdexcept>

namespace lua {
namespace exceptions {

struct NoSuchKey : public std::runtime_error {
    NoSuchKey(std::string problem): std::runtime_error(problem.c_str()) {}
};

struct CouldNotParse : public std::runtime_error {
    CouldNotParse(std::string problem): std::runtime_error(problem.c_str()) {}
};

} // namespace exceptions
} // namespace lua

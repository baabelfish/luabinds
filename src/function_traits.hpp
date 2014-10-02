#pragma once

#include <utility>

namespace tmp {

template<typename F>
struct function_traits;

template<typename R, typename... Args>
struct function_traits<R(Args...)> {
    using return_type = R;
    using param_types = std::tuple<Args...>;
};

template<typename T, typename R, typename... Args>
struct function_traits<R(T::*)(Args...) const> {
    using return_type = R;
    using param_types = std::tuple<Args...>;
};

template<typename R, typename... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)> {};

template<typename Class, typename R>
struct function_traits<R(Class::*)> {};

template<typename Class, typename R, typename... Args>
struct function_traits<R(Class::*)(Args...)> : public function_traits<R(Class&, Args...)> {};

template<typename T> struct function_traits : public function_traits<decltype(&T::operator())> {};

} // namespace tmp

#pragma once

#include "string_view.hpp"

namespace hpp
{

template <typename T>
constexpr hpp::string_view type_name();

template <>
constexpr hpp::string_view type_name<void>()
{
    return "void";
}

namespace detail
{

using type_name_prober = void;

template <typename T>
constexpr hpp::string_view wrapped_type_name()
{
#if defined(__clang__)
    return __PRETTY_FUNCTION__;
#elif defined(__GNUC__) && !defined(__clang__)
#define __HPP_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#if (__HPP_GCC_VERSION <= 90100)
    return __builtin_FUNCTION();
#else
    return __PRETTY_FUNCTION__;
#endif
#elif defined(_MSC_VER)
    return __FUNCSIG__;
#else
    #error "No support for this compiler."
#endif
}

constexpr std::size_t wrapped_type_name_prefix_length()
{
    return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>());
}

constexpr std::size_t wrapped_type_name_suffix_length()
{
    return wrapped_type_name<type_name_prober>().length()
         - wrapped_type_name_prefix_length()
         - type_name<type_name_prober>().length();
}

} // namespace detail

template <typename T>
constexpr hpp::string_view type_name()
{
    constexpr auto wrapped_name = detail::wrapped_type_name<T>();
    constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
    constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
    constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
    return wrapped_name.substr(prefix_length, type_name_length);
}

template <typename T>
std::string type_name_str()
{
    return std::string(type_name<T>());
}

}// namespace hpp

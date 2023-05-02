#pragma once

#include "traits/function_traits.hpp"
#include "traits/integral_constant.hpp"
#include "traits/is_detected.hpp"
#include "traits/logical.hpp"

namespace hpp
{

inline bool check_all_true()
{
	return true;
}

template <typename... BoolArgs>
inline bool check_all_true(bool arg1, BoolArgs... args)
{
	return arg1 & check_all_true(args...);
}

template <bool...>
struct bool_pack;
template <bool... v>
using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;
} // namespace hpp

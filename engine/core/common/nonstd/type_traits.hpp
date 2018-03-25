#ifndef NONSTD_TYPE_TRAITS
#define NONSTD_TYPE_TRAITS

#include "traits/integral_constant.hpp"
#include "traits/is_detected.hpp"
#include "traits/logical.hpp"
#include "traits/function_traits.hpp"

namespace nonstd
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

}

#endif

#ifndef NONSTD_TYPE_TRAITS
#define NONSTD_TYPE_TRAITS

#include <cstddef>
#include <future>
#include <type_traits>

namespace nonstd
{
template <class>
struct is_reference_wrapper : std::false_type
{
};

template <class T>
struct is_reference_wrapper<std::reference_wrapper<T>> : std::true_type
{
};

template <class T>
struct decay_reference_wrapper
{
	using type = T;
};

template <class T>
struct decay_reference_wrapper<std::reference_wrapper<T>>
{
	using type = T;
};

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

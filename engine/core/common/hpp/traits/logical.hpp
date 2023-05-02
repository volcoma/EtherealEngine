#pragma once
#include "integral_constant.hpp"
#include <cstddef>
#include <type_traits>

namespace hpp
{
/// CONJUNCTION
template <class...>
struct conjunction : std::true_type
{
};
template <class B1>
struct conjunction<B1> : B1
{
};
template <class B1, class... Bn>
struct conjunction<B1, Bn...> : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1>
{
};

template <class... B>
constexpr bool conjunction_v = conjunction<B...>::value;

/// DISJUNCTION
template <class...>
struct disjunction : std::false_type
{
};
template <class B1>
struct disjunction<B1> : B1
{
};
template <class B1, class... Bn>
struct disjunction<B1, Bn...> : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>
{
};

template <class... B>
constexpr bool disjunction_v = disjunction<B...>::value;

/// NEGATION
template <class B>
struct negation : bool_constant<!bool(B::value)>
{
};

template <class B>
constexpr bool negation_v = negation<B>::value;
}

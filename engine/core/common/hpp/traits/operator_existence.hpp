#pragma once

#include <type_traits>

namespace hpp
{
namespace operator_existence
{

struct No {};

#define OPERATOR_CREATOR(name, op)                                                                                          \
namespace impl                                                                                                              \
{                                                                                                                           \
template<typename T, typename Arg> No operator op (const T&, const Arg&);                                                   \
template<typename T, typename Arg, typename T3=void>                                                                        \
struct name##_impl                                                                                                          \
{                                                                                                                           \
    enum { value = false };                                                                                                 \
};                                                                                                                          \
template<typename T, typename Arg>                                                                                          \
struct name##_impl<T, Arg, typename std::enable_if_t<!std::is_fundamental<T>::value || !std::is_fundamental<Arg>::value>>   \
{                                                                                                                           \
    enum { value = !std::is_same<decltype(std::declval<T>() op std::declval<Arg>()), No>::value };                          \
};                                                                                                                          \
}                                                                                                                           \
template<typename T, typename Arg = T>                                                                                      \
struct name                                                                                                                 \
{                                                                                                                           \
    enum { value = impl::name##_impl<T, Arg>::value };                                                                      \
};                                                                                                                          \

OPERATOR_CREATOR(addition, +)
OPERATOR_CREATOR(substraction, -)
OPERATOR_CREATOR(multiplication, *)
OPERATOR_CREATOR(division, /)
OPERATOR_CREATOR(equal, ==)
OPERATOR_CREATOR(not_equal, !=)
OPERATOR_CREATOR(greater, >)
OPERATOR_CREATOR(less, <)
OPERATOR_CREATOR(less_or_equal, <=)
OPERATOR_CREATOR(greater_or_equal, >=)

#undef OPERATOR_CREATOR

}
}

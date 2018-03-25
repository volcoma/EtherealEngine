#pragma once
#include "invoke.hpp"
#include <tuple>

namespace nonstd
{
namespace detail
{

/*
 * apply implemented as per the C++17 standard specification.
 */
template <class F, class T, std::size_t... I>
constexpr inline decltype(auto) apply_(F&& f, T&& t, std::index_sequence<I...>) noexcept(
	noexcept(nonstd::invoke(std::forward<F>(f), std::get<I>(std::forward<T>(t))...)))
{
	return nonstd::invoke(std::forward<F>(f), std::get<I>(std::forward<T>(t))...);
}

} // namespace detail

template <class F, class T>
constexpr inline decltype(auto) apply(F&& f, T&& t) noexcept(noexcept(
	detail::apply_(std::forward<F>(f), std::forward<T>(t),
				   std::make_index_sequence<std::tuple_size<typename std::decay<T>::type>::value>{})))
{
	return detail::apply_(std::forward<F>(f), std::forward<T>(t),
						  std::make_index_sequence<std::tuple_size<typename std::decay<T>::type>::value>{});
}
}

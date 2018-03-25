#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>
namespace nonstd
{
namespace detail
{
template <class T>
struct is_reference_wrapper : std::false_type
{
};
template <class U>
struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
{
};
/*
 * invoke implemented as per the C++17 standard specification.
 */
template <class B, class T, class D, class... Args>
constexpr inline auto
invoke_(T B::*f, D&& d,
		Args&&... args) noexcept(noexcept((std::forward<D>(d).*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								std::is_base_of<B, typename std::decay<D>::type>::value,
							decltype((std::forward<D>(d).*f)(std::forward<Args>(args)...))>::type
{
	return (std::forward<D>(d).*f)(std::forward<Args>(args)...);
}

template <class B, class T, class R, class... Args>
constexpr inline auto invoke_(T B::*f, R&& r,
							  Args&&... args) noexcept(noexcept((r.get().*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								is_reference_wrapper<typename std::decay<R>::type>::value,
							decltype((r.get().*f)(std::forward<Args>(args)...))>::type
{
	return (r.get().*f)(std::forward<Args>(args)...);
}

template <class B, class T, class P, class... Args>
constexpr inline auto
invoke_(T B::*f, P&& p,
		Args&&... args) noexcept(noexcept(((*std::forward<P>(p)).*f)(std::forward<Args>(args)...))) ->
	typename std::enable_if<std::is_function<T>::value &&
								!is_reference_wrapper<typename std::decay<P>::type>::value &&
								!std::is_base_of<B, typename std::decay<P>::type>::value,
							decltype(((*std::forward<P>(p)).*f)(std::forward<Args>(args)...))>::type
{
	return ((*std::forward<P>(p)).*f)(std::forward<Args>(args)...);
}

template <class B, class T, class D>
constexpr inline auto invoke_(T B::*m, D&& d) noexcept(noexcept(std::forward<D>(d).*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								std::is_base_of<B, typename std::decay<D>::type>::value,
							decltype(std::forward<D>(d).*m)>::type
{
	return std::forward<D>(d).*m;
}

template <class B, class T, class R>
constexpr inline auto invoke_(T B::*m, R&& r) noexcept(noexcept(r.get().*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								is_reference_wrapper<typename std::decay<R>::type>::value,
							decltype(r.get().*m)>::type
{
	return r.get().*m;
}

template <class B, class T, class P>
constexpr inline auto invoke_(T B::*m, P&& p) noexcept(noexcept((*std::forward<P>(p)).*m)) ->
	typename std::enable_if<!std::is_function<T>::value &&
								!is_reference_wrapper<typename std::decay<P>::type>::value &&
								!std::is_base_of<B, typename std::decay<P>::type>::value,
							decltype((*std::forward<P>(p)).*m)>::type
{
	return (*std::forward<P>(p)).*m;
}

template <class Callable, class... Args>
constexpr inline auto
invoke_(Callable&& c,
		Args&&... args) noexcept(noexcept(std::forward<Callable>(c)(std::forward<Args>(args)...)))
	-> decltype(std::forward<Callable>(c)(std::forward<Args>(args)...))
{
	return std::forward<Callable>(c)(std::forward<Args>(args)...);
}

} // namespace detail

template <class F, class... Args>
constexpr inline auto
invoke(F&& f,
	   Args&&... args) noexcept(noexcept(detail::invoke_(std::forward<F>(f), std::forward<Args>(args)...)))
	-> decltype(detail::invoke_(std::forward<F>(f), std::forward<Args>(args)...))
{
	return detail::invoke_(std::forward<F>(f), std::forward<Args>(args)...);
}

// Conforming C++14 implementation (is also a valid C++11 implementation):
namespace detail
{
template <typename AlwaysVoid, typename, typename...>
struct invoke_result
{
};
template <typename F, typename... Args>
struct invoke_result<decltype(void(invoke(std::declval<F>(), std::declval<Args>()...))), F, Args...>
{
	using type = decltype(invoke(std::declval<F>(), std::declval<Args>()...));
};
} // namespace detail

template <class>
struct result_of;
template <class F, class... ArgTypes>
struct result_of<F(ArgTypes...)> : detail::invoke_result<void, F, ArgTypes...>
{
};

template<typename F, typename ...Args>
using result_of_t = typename result_of<F(Args...)>::type;

template <class F, class... ArgTypes>
struct invoke_result : detail::invoke_result<void, F, ArgTypes...>
{
};

template<typename F, typename ...Args>
using invoke_result_t = typename invoke_result<F, Args...>::type;

}

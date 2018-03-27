#pragma once
#include "../common/nonstd/type_traits.hpp"
#include "../common/nonstd/utility.hpp"

#include <chrono>
#include <future>
namespace core
{
template <typename T>
class task_future;
namespace async
{
namespace detail
{

template <typename T>
using has_get = decltype(std::declval<T>().get());
template <typename T>
using has_wait = decltype(std::declval<T>().wait());
template <typename T>
using has_valid = decltype(std::declval<T>().valid());
template <typename T>
using has_wait_for = decltype(std::declval<T>().wait_for(std::declval<std::chrono::milliseconds>()));
template <typename T>
using has_wait_until =
	decltype(std::declval<T>().wait_until(std::declval<std::chrono::system_clock::time_point>()));

namespace v2
{

template <typename T>
using is_future_impl =
	nonstd::conjunction<nonstd::is_detected<has_get, T>, nonstd::is_detected<has_wait, T>,
						nonstd::is_detected<has_valid, T>, nonstd::is_detected<has_wait_for, T>,
						nonstd::is_detected<has_wait_until, T>>;

template <typename T>
using is_future = is_future_impl<std::decay_t<T>>;

template <typename T>
struct decay_future
{
	using type = T;
};

template <template <typename> class X, typename T>
struct decay_future<X<T>>
{
	using type = T;
};
template <template <typename> class X, typename T>
struct decay_future<X<T>&>
{
	using type = T;
};

template <template <typename> class X, typename T>
struct decay_future<const X<T>&>
{
	using type = T;
};
}

namespace impl = v2;

template <typename T>
using is_future = impl::is_future<T>;

template <typename T>
constexpr bool is_future_v = is_future<T>::value;

template <typename T>
using decay_future = impl::decay_future<T>;

template <typename T>
using decay_future_t = typename decay_future<T>::type;
}
}
}

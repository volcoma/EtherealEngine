#pragma once

#include <type_traits>
#include <utility>

namespace hpp
{

namespace cexpr
{
template <bool predicate>
using bool_constant = std::integral_constant<bool, predicate>;

struct caller_false
{
	template <bool E, typename F>
	caller_false call(bool_constant<E>, F&&)
	{
		return {};
	}
};

struct caller_true
{
	template <typename F>
	caller_false call(bool_constant<true> tag, F&& f)
	{
		f(tag);
		return {};
	}
	template <typename F>
	caller_true call(bool_constant<false>, F&&)
	{
		return {};
	}
};
struct avoid_user_return
{
	explicit avoid_user_return() = default;

	template <typename T, typename = std::enable_if_t<!std::is_same<T, avoid_user_return>::value>>
	avoid_user_return(T&&)
	{
		constexpr const bool check = !std::is_same<T, T>::value;
		static_assert(check, "It is forbidden to use a return statement in this if_constexpr(...) block."
							 "Forbidden statements are : 'return', 'break', 'continue' and 'goto'");
	}
};
} // namespace cexpr
} // namespace hpp

#define constexpr_impl_begin_branch [&](auto) -> hpp::cexpr::avoid_user_return {
#define constexpr_impl_end_branch                                                                            \
	return hpp::cexpr::avoid_user_return{};                                                                  \
	} )

#define if_constexpr(...) hpp::cexpr::caller_true().call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), constexpr_impl_begin_branch
#define else_if_constexpr(...) constexpr_impl_end_branch.call(hpp::cexpr::bool_constant<(__VA_ARGS__)>(), constexpr_impl_begin_branch
#define else_constexpr constexpr_impl_end_branch.call(hpp::cexpr::bool_constant<true>(), constexpr_impl_begin_branch
#define end_if_constexpr constexpr_impl_end_branch

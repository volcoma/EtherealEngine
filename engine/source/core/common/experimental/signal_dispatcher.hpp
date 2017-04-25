#pragma once

#include <memory> 
#include <string> 
#include <vector> 
#include <unordered_map> 
#include <functional> 
#include <tuple>
#include <cassert>
#include <typeindex>

//remove when std::any comes along
#include "../any.hpp"
using any_t = core::any;
#define any_cast_t = core::any_cast

template<typename TupleType, typename FunctionType>
void for_each(TupleType&&, FunctionType
	, std::integral_constant<size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>) {}

template<std::size_t I, typename TupleType, typename FunctionType
	, typename = typename std::enable_if<I != std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type >
	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
{
	f(std::get<I>(t));
	for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
}

template<typename TupleType, typename FunctionType>
void for_each(TupleType&& t, FunctionType f)
{
	for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
}

template<typename F, typename Tuple, std::size_t ... I>
auto apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
	return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}
template<typename F, typename Tuple>
auto apply(F&& f, Tuple&& t)
{
	using Indices = std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;
	return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices());
}

template <typename... Args>
std::vector<any_t> fill_args(Args&&... args)
{
	return{ any_t(args)... };
}

template <typename T>
struct function_traits : function_traits<decltype(&T::operator())> {};

#define REM_CTOR(...) __VA_ARGS__
#define SPEC(cv, var, is_var)												\
template <typename C, typename R, typename... Args>							\
struct function_traits<R (C::*) (Args... REM_CTOR var) cv>                  \
{																			\
    using arity = std::integral_constant<std::size_t, sizeof...(Args) >;	\
    using is_variadic = std::integral_constant<bool, is_var>;				\
    using is_const    = std::is_const<int cv>;								\
																			\
    using result_type = R;													\
																			\
    template <std::size_t i>												\
    using arg = typename std::tuple_element<i, std::tuple<Args...>>::type;	\
	using args_tuple = std::tuple<Args...>;									\
	using fn = const std::function<R(Args...)>;								\
	using is_void = std::is_same<R, void>;									\
};

SPEC(const, (, ...), 1)
SPEC(const, (), 0)
SPEC(, (, ...), 1)
SPEC(, (), 0)

template<typename From, typename To>
To implicit_cast_impl(const any_t& operand)
{
	auto val = any_cast_t<From>(operand);
	return static_cast<To>(val);
}

template<typename T>
T implicit_cast(const any_t& operand)
{
	const auto& from = operand.type();
	const auto& to = typeid(T);
	if (from == typeid(std::int8_t))
	{
		return implicit_cast_impl<std::int8_t, T>(operand);
	}
	else if (from == typeid(std::int16_t))
	{
		return implicit_cast_impl<std::int16_t, T>(operand);
	}
	else if (from == typeid(std::int32_t))
	{
		return implicit_cast_impl<std::int32_t, T>(operand);
	}
	else if (from == typeid(std::int64_t))
	{
		return implicit_cast_impl<std::int64_t, T>(operand);
	}
	else if (from == typeid(std::uint8_t))
	{
		return implicit_cast_impl<std::uint8_t, T>(operand);
	}
	else if (from == typeid(std::uint16_t))
	{
		return implicit_cast_impl<std::uint16_t, T>(operand);
	}
	else if (from == typeid(std::uint32_t))
	{
		return implicit_cast_impl<std::uint32_t, T>(operand);
	}
	else if (from == typeid(std::uint64_t))
	{
		return implicit_cast_impl<std::uint64_t, T>(operand);
	}
	else if (from == typeid(float))
	{
		return implicit_cast_impl<float, T>(operand);
	}
	else if (from == typeid(double))
	{
		return implicit_cast_impl<double, T>(operand);
	}

	assert(from == to && "cannot implicitly cast types");
	return any_cast_t<T>(operand);
}

struct function_wrapper
{
	virtual ~function_wrapper() = default;
	virtual void invoke(const std::vector<any_t>& params, bool supports_implicit_casting) const = 0;
};

template<typename F>
class function_wrapper_t : public function_wrapper
{

public:
	function_wrapper_t(F&& f) : _function(f) {}

	virtual void invoke(const std::vector<any_t>& params, bool supports_implicit_casting) const
	{
		function_traits<F>::args_tuple args;
		assert(function_traits<F>::arity::value <= params.size() && "not enough arguments");
		std::size_t i = 0;
		for_each(args, [&](auto& arg)
		{
			using arg_type = typename std::decay<decltype(arg)>::type;
			const auto& param = params[i++];
			const auto& t1 = param.type();
			const auto& t2 = typeid(arg_type);

			if (supports_implicit_casting)
			{
				arg = implicit_cast<arg_type>(param);
			}
			else
			{
				assert(t1 == t2 && "different types!");
				arg = any_cast_t<arg_type>(param);
			}
		});
		apply(_function, args);
	}
private:
	typename function_traits<F>::fn _function;
};

template <typename F>
std::unique_ptr<function_wrapper> create_wrapper(F&& f)
{
	return std::unique_ptr<function_wrapper_t<decltype(f)>>(new function_wrapper_t<decltype(f)>(std::forward<F>(f)));
}

template<bool implicit_casting = true>
class signal_dispatcher
{
public:
	template<typename F>
	void connect(const std::string& name, F f)
	{
		static_assert(std::is_same<void, typename function_traits<F>::result_type>::value,
			"Signals cannot have a return type different from void");

		_list[name].emplace_back(create_wrapper(std::forward<F>(f)));
	}

	template<typename ... Args>
	void emit(const std::string& name, Args&&... args)
	{
		//copy to protect from iterator invalidation if someone connects
		auto& funcs = _list[name];

		for (const auto& func : funcs)
		{
			func->invoke(fill_args(std::forward<Args>(args) ...), implicit_casting);
		}
	}
private:
	std::unordered_map<std::string, std::vector<std::unique_ptr<function_wrapper>>> _list;

};
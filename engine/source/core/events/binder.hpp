#pragma once

#include "../common/any.hpp"
#include "../common/function_traits.hpp"
#include "delegate.hpp"

#include <memory> 
#include <string> 
#include <vector> 
#include <unordered_map> 
#include <functional> 
#include <tuple>
#include <cassert>
#include <typeindex>
#include <algorithm>

using any_t = nonstd::util::any;
#define any_cast_t nonstd::util::any_cast

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

template<typename F, typename Tuple, typename std::enable_if<!std::is_same<typename core::fn_result_of<F>, void>::value>::type* = nullptr>
any_t call(F&& f, Tuple&& t)
{
	return apply(std::forward<F>(f), std::forward<Tuple>(t));
}
template<typename F, typename Tuple, typename std::enable_if<std::is_same<typename core::fn_result_of<F>, void>::value>::type* = nullptr>
any_t call(F&& f, Tuple&& t)
{
	any_t result;
	apply(std::forward<F>(f), std::forward<Tuple>(t));
	return result;
}

template <typename... Args>
std::vector<any_t> fill_args(Args&&... args)
{
	return{ any_t(args)... };
}

template<typename From, typename To, typename std::enable_if<!std::is_convertible<From, To>::value>::type* = nullptr>
To implicit_cast_impl(const any_t& operand)
{
	return To{};
}

template<typename From, typename To, typename std::enable_if<std::is_convertible<From, To>::value>::type* = nullptr>
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
	else if (from == typeid(const char*))
	{
		return implicit_cast_impl<const char*, T>(operand);
	}
	else if (from == typeid(std::nullptr_t))
	{
		if (to != typeid(std::string))
			return implicit_cast_impl<std::nullptr_t, T>(operand);
	}

	assert(from == to && "cannot implicitly cast types");
	return any_cast_t<T>(operand);
}

struct function_wrapper
{
	virtual ~function_wrapper() = default;
	virtual any_t invoke(const std::vector<any_t>& params) const = 0;
	virtual bool owns(const any_t& delegate) const = 0;
};

template<typename F>
class function_wrapper_t : public function_wrapper
{
public:
	using delegate_t = delegate<typename core::function_traits<F>::function_type>;

	template<typename C>
	function_wrapper_t(C * const object_ptr, F f)
		: _function(object_ptr, f)
	{}

	function_wrapper_t(F f)
		: _function(f)
	{}

	virtual any_t invoke(const std::vector<any_t>& params) const
	{
		core::function_traits<F>::tuple_type_decayed args;
		const auto arity = core::function_traits<F>::arity;
		std::size_t i = 0;
		for_each(args, [&](auto& arg)
		{
			using arg_type = core::special_decay_t<decltype(arg)>;

			if (i >= params.size())
			{
				arg = arg_type{};
				i++;
				return;
			}

			const auto& param = params[i++];
			const auto& t1 = param.type();
			const auto& t2 = typeid(arg_type);

			arg = implicit_cast<arg_type>(param);

		});

		return call(_function, args);
	}

	virtual bool owns(const any_t& any_delegate) const
	{
		if (any_delegate.type() == typeid(delegate_t))
		{
			auto del = any_cast_t<delegate_t>(any_delegate);
			return del == _function;
		}
		return false;
	}

private:

	delegate_t _function;
};


template <typename F>
std::unique_ptr<function_wrapper> create_wrapper(F f)
{
	return std::unique_ptr<function_wrapper_t<F>>(new function_wrapper_t<F>(std::forward<F>(f)));
}

template <typename C, typename F>
std::unique_ptr<function_wrapper> create_wrapper(C * const object_ptr, F f)
{
	return std::unique_ptr<function_wrapper_t<F>>(new function_wrapper_t<F>(object_ptr, std::forward<F>(f)));
}

class signal_binder
{
public:
	template<typename F>
	void connect(const std::string& name, F f)
	{
		static_assert(std::is_same<void, typename core::function_traits<F>::result_type>::value,
			"Signals cannot have a return type different from void");

		_list[name].emplace_back(create_wrapper(std::forward<F>(f)));
	}

	template<typename C, typename F>
	void connect(const std::string& name, C * const object_ptr, F f)
	{
		static_assert(std::is_same<void, typename core::function_traits<F>::result_type>::value,
			"Signals cannot have a return type different from void");

		_list[name].emplace_back(create_wrapper(object_ptr, std::forward<F>(f)));
	}

	template<typename C, typename F>
	void disconnect(const std::string& name, C * const object_ptr, F f)
	{
		static_assert(std::is_same<void, typename core::function_traits<F>::result_type>::value,
			"Signals cannot have a return type different from void");

		auto& funcs = _list[name];

		auto slot = function_wrapper_t<F>::delegate_t(object_ptr, f);
		funcs.erase(std::remove_if(std::begin(funcs), std::end(funcs),
			[&slot](const auto& other) { return other->owns(slot); }
		), std::end(funcs));

		//If it was the last one, remove the whole container
		if (funcs.empty())
			_list.erase(name);
	}

	template<typename ... Args>
	void emit(const std::string& name, Args&&... args)
	{
		const auto& funcs = _list[name];

		//Iterate this way to allow modification
		for (size_t i = 0, count = funcs.size(); i < count; ++i)
		{
			const auto& func = funcs[i];
			func->invoke(fill_args(std::forward<Args>(args) ...));
		}
	}
private:
	/// signal / slots
	std::unordered_map<std::string, std::vector<std::unique_ptr<function_wrapper>>> _list;

};

class delegate_binder
{
public:
	template<typename F>
	void bind(const std::string& name, F f)
	{
		_list[name] = create_wrapper(std::forward<F>(f));
	}

	template<typename C, typename F>
	void bind(const std::string& name, C * const object_ptr, F f)
	{
		_list[name] = create_wrapper(object_ptr, std::forward<F>(f));
	}

	void unbind(const std::string& name)
	{
		_list.erase(name);
	}

	template<typename R, typename ... Args, typename std::enable_if<!std::is_same<R, void>::value>::type* = nullptr>
	R call(const std::string& name, Args&&... args)
	{
		const auto& func = _list[name];
		auto result = func->invoke(fill_args(std::forward<Args>(args) ...));

		assert(result.type() == typeid(R) && "invalid return type");
		return any_cast_t<R>(result);
	}
	template<typename R, typename ... Args, typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
	R call(const std::string& name, Args&&... args)
	{
		const auto& func = _list[name];
		func->invoke(fill_args(std::forward<Args>(args) ...));
	}

private:
	/// signal / slots
	std::unordered_map<std::string, std::unique_ptr<function_wrapper>> _list;

};

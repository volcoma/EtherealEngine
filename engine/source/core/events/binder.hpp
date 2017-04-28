#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <cassert>
#include <typeindex>
#include <algorithm>
#include <type_traits>
#include "../common/any.hpp"
#include "../common/optional.hpp"
#include "../common/function_traits.hpp"
#include "../events/delegate.hpp"

using any_t = nonstd::any;
#define any_cast_t nonstd::any_cast

template<typename T>
using optional_t = nonstd::optional<T>;

template <typename T>
using function_traits_t = typename nonstd::function_traits<T>;

template <typename T>
using fn_result_of_t = typename nonstd::fn_result_of<T>;

template <typename T>
using special_decay_t = typename nonstd::special_decay_t<T>;

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

template<typename F, typename Tuple, typename std::enable_if<!std::is_same<fn_result_of_t<F>, void>::value>::type* = nullptr>
any_t call(F&& f, Tuple&& t)
{
	return apply(std::forward<F>(f), std::forward<Tuple>(t));
}
template<typename F, typename Tuple, typename std::enable_if<std::is_same<fn_result_of_t<F>, void>::value>::type* = nullptr>
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
bool implicit_cast_impl(const any_t& operand, To& result)
{
	return false;
}

template<typename From, typename To, typename std::enable_if<std::is_convertible<From, To>::value>::type* = nullptr>
bool implicit_cast_impl(const any_t& operand, To& result)
{
	auto val = any_cast_t<From>(operand);
	result = static_cast<To>(val);
	return true;
}

template<typename T>
bool implicit_cast(const any_t& operand, T& result)
{
	const auto& from = operand.type();
	const auto& to = typeid(T);

	if (from == to)
	{
		result = any_cast_t<T>(operand);
		return true;
	}
	else if (from == typeid(std::int8_t) && implicit_cast_impl<std::int8_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::int16_t) && implicit_cast_impl<std::int16_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::int32_t) && implicit_cast_impl<std::int32_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::int64_t) && implicit_cast_impl<std::int64_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::uint8_t) && implicit_cast_impl<std::uint8_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::uint16_t) && implicit_cast_impl<std::uint16_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::uint32_t) && implicit_cast_impl<std::uint32_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::uint64_t) && implicit_cast_impl<std::uint64_t, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(float) && implicit_cast_impl<float, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(double) && implicit_cast_impl<double, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(const char*) && implicit_cast_impl<const char*, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(char) && implicit_cast_impl<char, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(unsigned char) && implicit_cast_impl<unsigned char, T>(operand, result))
	{
		return true;
	}
	else if (from == typeid(std::nullptr_t))
	{
		if (to != typeid(std::string))
			return implicit_cast_impl<std::nullptr_t, T>(operand, result);
	}

	return false;
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
	using delegate_t = delegate<typename function_traits_t<F>::function_type>;

	template<typename C>
	function_wrapper_t(C * const object_ptr, F f)
		: _function(object_ptr, f)
	{}

	function_wrapper_t(F f)
		: _function(f)
	{}

	virtual any_t invoke(const std::vector<any_t>& params) const
	{
		typename function_traits_t<F>::tuple_type_decayed args;
		//const auto arity = function_traits_t<F>::arity;
		//assert(arity <= params.size() && "less parameters are not allowed");
		std::size_t i = 0;
		for_each(args, [&](auto& arg)
		{
			using arg_type = special_decay_t<decltype(arg)>;

			if (i >= params.size())
			{
				i++;
				return;
			}

			const auto& param = params[i++];
			bool can_cast = implicit_cast(param, arg);
			assert(can_cast && "cannot implicitly convert types");
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

template<typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class signals_t
{
	struct info_t
	{
		optional_t<sentinel_t> sentinel;
		std::unique_ptr<function_wrapper> function;
	};

public:
	template<typename F>
	void connect(const id_t& id, F f)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.function = create_wrapper(std::forward<F>(f));
		_list[id].emplace_back(std::move(info));
	}

	template<typename C, typename F>
	void connect(const id_t& id, C * const object_ptr, F f)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));
		_list[id].emplace_back(std::move(info));
	}

	template<typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, F f)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.sentinel = sentinel;
		info.function = create_wrapper(std::forward<F>(f));
		_list[id].emplace_back(std::move(info));
	}

	template<typename C, typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, C * const object_ptr, F f)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.sentinel = sentinel;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));
		_list[id].emplace_back(std::move(info));
	}

	template<typename C, typename F>
	void disconnect(const id_t& id, C * const object_ptr, F f)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		auto& container = _list[id];

		auto slot = typename function_wrapper_t<F>::delegate_t(object_ptr, f);
		container.erase(std::remove_if(std::begin(container), std::end(container),
			[&slot](const auto& info) { return info.function->owns(slot); }
		), std::end(container));

		//If it was the last one, remove the whole container
		if (container.empty())
			_list.erase(id);
	}

	template<typename ... Args>
	void emit(const id_t& id, Args&&... args)
	{
		auto& container = _list[id];

		//remove expired subscribers
		container.erase(std::remove_if(std::begin(container), std::end(container),
			[](const auto& info)
		{
			return info.sentinel.has_value() && info.sentinel.value().expired();
		}), std::end(container));

		auto any_args = fill_args(std::forward<Args>(args) ...);
		//Iterate this way to allow modification
		for (size_t i = 0; i < container.size(); ++i)
		{
			const auto& info = container[i];
			info.function->invoke(any_args);
		}
	}

	void clear()
	{
		_list.clear();
	}

private:
	/// signal / slots
	std::unordered_map<id_t, std::vector<info_t>> _list;

};

template<typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class functions_t
{
	struct info_t
	{
		optional_t<sentinel_t> sentinel;
		std::unique_ptr<function_wrapper> function;
	};
public:
	template<typename F>
	void bind(const id_t& id, F f)
	{
		info_t info;
		info.function = create_wrapper(std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template<typename C, typename F>
	void bind(const id_t& id, C * const object_ptr, F f)
	{
		info_t info;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template<typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, F f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = create_wrapper(std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template<typename C, typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, C * const object_ptr, F f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));
		_list[id] = std::move(info);
	}

	void unbind(const id_t& id)
	{
		_list.erase(id);
	}

	template<typename R, typename ... Args, typename std::enable_if<!std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args)
	{
		static_assert(!std::is_reference<R>::value, "unsupported return by reference (use return by value)");

		R res{};

		auto it = _list.find(id);
		if (it == _list.end())
			return res;

		const auto& info = it->second;

		// check if subscriber expired
		if (info.sentinel.has_value() && info.sentinel.value().expired())
		{
			_list.erase(it);
			return res;
		}

		if (info.function != nullptr)
		{
			auto result = info.function->invoke(fill_args(std::forward<Args>(args) ...));
			bool can_cast = implicit_cast<R>(result, res);
			assert(can_cast && "cannot implicitly convert types");
		}
		return res;
	}
	template<typename R, typename ... Args, typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args)
	{
		static_assert(!std::is_reference<R>::value, "unsupported return by reference (use return by value)");

		auto it = _list.find(id);
		if (it == _list.end())
			return;

		const auto& info = it->second;

		// check if subscriber expired
		if (info.sentinel.has_value() && info.sentinel.value().expired())
		{
			_list.erase(it);
			return;
		}

		if (info.function != nullptr)
		{
			info.function->invoke(fill_args(std::forward<Args>(args) ...));
		}
	}

	void clear()
	{
		_list.clear();
	}

private:
	/// signal / slots
	std::unordered_map<id_t, info_t> _list;

};


template<typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
struct binder_t
{
	signals_t<id_t, sentinel_t> signals;
	functions_t<id_t, sentinel_t> functions;
};
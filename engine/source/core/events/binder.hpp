#ifndef _BINDER_HPP_
#define _BINDER_HPP_

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <cassert>
#include <algorithm>
#include <type_traits>
#include "../common/any.hpp"
#include "../common/optional.hpp"
#include "../common/function_traits.hpp"
#include "delegate.hpp"

using any_t = nonstd::any;
#define any_cast_t nonstd::any_cast

template<typename T>
using optional_t = nonstd::optional<T>;

template <typename T>
using function_traits_t = typename nonstd::function_traits<T>;

template<typename T, size_t Index>
using param_types_decayed_t = typename nonstd::param_types_decayed<T, Index>::type;

template <typename T>
using fn_result_of_t = typename nonstd::fn_result_of<T>;

template<typename T, typename Check>
using fn_result_is_t = std::is_same<typename nonstd::fn_result_of<T>, Check>;

template <typename T>
using special_decay_t = typename nonstd::special_decay_t<T>;

template< bool B, class T, class F >
using conditional_t = typename std::conditional<B, T, F>::type;


struct void_func
{
	using type = void_func;
};

struct return_func
{
	using type = return_func;
};


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
	if (operand.type() == rtti::type_id<From>())
	{
		auto val = any_cast_t<From>(operand);
		result = static_cast<To>(val);
		return true;
	}
	
	return false;
}

template<typename To>
bool try_implicit_cast(const any_t& operand, To& result)
{
	if (operand.type() == rtti::type_id<To>())
	{
		auto val = any_cast_t<To>(operand);
		result = static_cast<To>(val);
		return true;
	}
	else if (implicit_cast_impl<std::int8_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::int16_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::int32_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::int64_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::uint8_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::uint16_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::uint32_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::uint64_t>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<float>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<double>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<const char*>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<char>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<unsigned char>(operand, result))
	{
		return true;
	}
	else if (implicit_cast_impl<std::nullptr_t>(operand, result))
	{
		return true;
	}

	return false;
}

template<typename T>
bool can_implicit_cast(const any_t& operand)
{
	T result;
	return try_implicit_cast<T>(operand, result);
}

template<typename T>
T implicit_cast(const any_t& operand)
{
	T result;
	try_implicit_cast<T>(operand, result);
	return result;
}


struct function_wrapper
{
	virtual ~function_wrapper() = default;

	virtual any_t invoke(const std::vector<any_t>& params) const = 0;

	virtual bool owns(const any_t& delegate) const = 0;
};

template<typename F, typename IndexSequence>
class function_wrapper_t;


template<typename F, size_t... ArgCount>
class function_wrapper_t<F, std::index_sequence<ArgCount...>> : public function_wrapper
{
	template<std::size_t... Arg_Idx>
	any_t invoke_variadic_impl(std::index_sequence<Arg_Idx...>, const std::vector<any_t>& arg_list) const
	{
		static const auto deduction_helper = conditional_t<fn_result_is_t<F, void>::value, void_func, return_func>::type();
		return invoke(deduction_helper, ((Arg_Idx < arg_list.size()) ? arg_list[Arg_Idx] : param_types_decayed_t<F, Arg_Idx>()) ...);
	}

	template<typename... Args>
	any_t invoke(const void_func&, const Args&...args) const
	{
		const bool all_params_are_convertible = nonstd::check_all_true(can_implicit_cast<param_types_decayed_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if (all_params_are_convertible)
		{
			_function(implicit_cast<param_types_decayed_t<F, ArgCount>>(args)...);
		}

		return any_t{};
	}


	template<typename... Args>
	any_t invoke(const return_func&, const Args&...args) const
	{
		const bool all_params_are_convertible = nonstd::check_all_true(can_implicit_cast<param_types_decayed_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if (all_params_are_convertible)
		{
			return _function(implicit_cast<param_types_decayed_t<F, ArgCount>>(args)...);
		}
		return any_t{};
	}

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
		return invoke_variadic_impl(std::make_index_sequence<function_traits_t<F>::arity>(), params);
	}

	virtual bool owns(const any_t& any_delegate) const
	{
		if (any_delegate.type() == rtti::type_id<delegate_t>())
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

	using arg_index_sequence = std::make_index_sequence< function_traits_t<F>::arity >;
	using wrapper = function_wrapper_t<F, arg_index_sequence>;
	return std::unique_ptr<wrapper>(new wrapper(std::forward<F>(f)));
}

template <typename C, typename F>
std::unique_ptr<function_wrapper> create_wrapper(C * const object_ptr, F f)
{
	using arg_index_sequence = std::make_index_sequence< function_traits_t<F>::arity >;
	using wrapper = function_wrapper_t<F, arg_index_sequence>;
	return std::unique_ptr<wrapper>(new wrapper(object_ptr, std::forward<F>(f)));
}

template<typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class signals_t
{
	struct info_t
	{
		std::uint32_t priority = 0;
		optional_t<sentinel_t> sentinel;
		std::unique_ptr<function_wrapper> function;
	};

public:
	template<typename F>
	void connect(const id_t& id, F f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.function = create_wrapper(std::forward<F>(f));
		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template<typename C, typename F>
	void connect(const id_t& id, C * const object_ptr, F f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));
		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template<typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, F f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.sentinel = sentinel;
		info.function = create_wrapper(std::forward<F>(f));
		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template<typename C, typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, C * const object_ptr, F f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename function_traits_t<F>::result_type>::value,
			"signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.sentinel = sentinel;
		info.function = create_wrapper(object_ptr, std::forward<F>(f));

		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
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

		check_sentinels(container);

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
	void sort(std::vector<info_t>& container)
	{
		std::sort(std::begin(container), std::end(container), [](const auto& info1, const auto& info2) {
			return info1.priority > info2.priority;
		});
	}

	void check_sentinels(std::vector<info_t>& container)
	{
		//remove expired subscribers
		container.erase(std::remove_if(std::begin(container), std::end(container),
			[](const auto& info)
		{
			return info.sentinel.has_value() && info.sentinel.value().expired();
		}), std::end(container));
	}

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
			bool can_cast = try_implicit_cast<R>(result, res);
			assert(can_cast && "cannot implicitly convert return type");
		}
		return res;
	}

	template<typename R, typename ... Args, typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args)
	{
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

#endif
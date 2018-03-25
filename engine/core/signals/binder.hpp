#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <vector>

#include "../common/nonstd/any.hpp"
#include "../common/nonstd/optional.hpp"
#include "../common/nonstd/type_traits.hpp"
#include "../common/nonstd/utility.hpp"
#include "delegate.hpp"

// Create vector of any from variadic pack
template <typename... Args>
std::vector<nonstd::any> fill_args(Args&&... args)
{
	return {nonstd::any(args)...};
}

// Non-convertible fallback
template <typename From, typename To,
		  typename std::enable_if<!std::is_convertible<From, To>::value>::type* = nullptr>
bool implicit_cast_impl(const nonstd::any& /*operand*/, To& /*result*/)
{
	return false;
}

// Convertible implicit cast
template <typename From, typename To,
		  typename std::enable_if<std::is_convertible<From, To>::value>::type* = nullptr>
bool implicit_cast_impl(const nonstd::any& operand, To& result)
{
	if(operand.type() == rtti::type_id<From>())
	{
		auto val = nonstd::any_cast<From>(operand);
		result = static_cast<To>(val);
		return true;
	}

	return false;
}

// Try to implicit cast an 'any' parameter to a type T
template <typename To>
bool try_implicit_cast(const nonstd::any& operand, To& result)
{
	if(operand.type() == rtti::type_id<To>())
	{
		auto val = nonstd::any_cast<To>(operand);
		result = static_cast<To>(val);
		return true;
	}
	if(implicit_cast_impl<std::int8_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::int16_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::int32_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::int64_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::uint8_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::uint16_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::uint32_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::uint64_t>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<float>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<double>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<const char*>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<char>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<unsigned char>(operand, result))
	{
		return true;
	}
	if(implicit_cast_impl<std::nullptr_t>(operand, result))
	{
		return true;
	}

	return false;
}

template <typename T>
bool can_implicit_cast(const nonstd::any& operand)
{
	T result;
	return try_implicit_cast<T>(operand, result);
}

template <typename T>
T implicit_cast(const nonstd::any& operand)
{
	T result;
	try_implicit_cast<T>(operand, result);
	return result;
}

// Function wrapper used for storage and invoke
struct base_function_wrapper
{
	virtual ~base_function_wrapper() = default;

	virtual nonstd::any invoke(const std::vector<nonstd::any>& params) const = 0;

	virtual bool owns(const nonstd::any& delegate) const = 0;
};

template <typename F, typename IndexSequence>
class function_wrapper;

template <typename F, size_t... ArgCount>
class function_wrapper<F, std::index_sequence<ArgCount...>> : public base_function_wrapper
{
	constexpr static const bool result_is_void = std::is_same<typename nonstd::fn_result_of<F>, void>::value;

	using is_void = std::integral_constant<bool, result_is_void>;

	template <std::size_t... Arg_Idx>
	nonstd::any invoke_variadic_impl(std::index_sequence<Arg_Idx...> /*seq*/,
									 const std::vector<nonstd::any>& arg_list) const
	{
		return invoke(is_void(),
					  ((Arg_Idx < arg_list.size()) ? arg_list[Arg_Idx]
												   : nonstd::param_types_decayed_t<F, Arg_Idx>())...);
	}

	// Invoke void function, "const Args&...args" are of type "any"
	template <typename... Args>
	nonstd::any invoke(std::true_type /*unused*/, const Args&... args) const
	{
		const auto all_params_are_convertible =
			nonstd::check_all_true(can_implicit_cast<nonstd::param_types_decayed_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if(all_params_are_convertible)
		{
			_function(implicit_cast<nonstd::param_types_decayed_t<F, ArgCount>>(args)...);
		}

		return nonstd::any{};
	}

	// Invoke non void function, "const Args&...args" are of type "any"
	template <typename... Args>
	nonstd::any invoke(std::false_type /*unused*/, const Args&... args) const
	{
		const auto all_params_are_convertible =
			nonstd::check_all_true(can_implicit_cast<nonstd::param_types_decayed_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if(all_params_are_convertible)
		{
			return _function(implicit_cast<nonstd::param_types_decayed_t<F, ArgCount>>(args)...);
		}
		return nonstd::any{};
	}

public:
	using delegate_t = delegate<typename nonstd::function_traits<F>::function_type>;

	template <typename C>
	function_wrapper(C* const object_ptr, F f)
		: _function(object_ptr, f)
	{
	}

	function_wrapper(F f)
		: _function(f)
	{
	}

	virtual nonstd::any invoke(const std::vector<nonstd::any>& params) const
	{
		return invoke_variadic_impl(std::make_index_sequence<nonstd::function_traits<F>::arity>(), params);
	}

	virtual bool owns(const nonstd::any& any_delegate) const
	{
		if(any_delegate.type() == rtti::type_id<delegate_t>())
		{
			auto del = nonstd::any_cast<delegate_t>(any_delegate);
			return del == _function;
		}
		return false;
	}

private:
	delegate_t _function;
};

template <typename F>
std::unique_ptr<base_function_wrapper> make_wrapper(F&& f)
{
	using arg_index_sequence = std::make_index_sequence<nonstd::function_traits<F>::arity>;
	using wrapper = function_wrapper<F, arg_index_sequence>;
	return std::make_unique<wrapper>(std::forward<F>(f));
}

template <typename C, typename F>
std::unique_ptr<base_function_wrapper> make_wrapper(C* const object_ptr, F&& f)
{
	using arg_index_sequence = std::make_index_sequence<nonstd::function_traits<F>::arity>;
	using wrapper = function_wrapper<F, arg_index_sequence>;
	return std::make_unique<wrapper>(object_ptr, std::forward<F>(f));
}

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class signals_t
{
	struct info_t
	{
		/// Priority used for sorting
		std::uint32_t priority = 0;
		/// Sentinel used for life tracking
		nonstd::optional<sentinel_t> sentinel;
		/// The function wrapper
		std::unique_ptr<base_function_wrapper> function;
	};

public:
	template <typename F>
	void connect(const id_t& id, F&& f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.function = make_wrapper(std::forward<F>(f));
		auto& container = list_[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename C, typename F>
	void connect(const id_t& id, C* const object_ptr, F f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));
		auto& container = list_[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, F&& f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.sentinel = sentinel;
		info.function = make_wrapper(std::forward<F>(f));
		auto& container = list_[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename C, typename F>
	void connect(const id_t& id, const sentinel_t& sentinel, C* const object_ptr, F&& f,
				 std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.sentinel = sentinel;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));

		auto& container = list_[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename C, typename F>
	void disconnect(const id_t& id, C* const object_ptr, F&& f)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		auto& container = list_[id];

		using arg_index_sequence = std::make_index_sequence<nonstd::function_traits<F>::arity>;
		auto slot =
			typename function_wrapper<F, arg_index_sequence>::delegate_t(object_ptr, std::forward<F>(f));
		auto it = std::find_if(std::begin(container), std::end(container),
							   [&slot](const info_t& info) { return info.function->owns(slot); });

		if(it != std::end(container))
		{
			auto& info = *it;
			info.sentinel = sentinel_t();
		}
	}

	template <typename... Args>
	void emit(const id_t& id, Args&&... args)
	{
		auto& container = list_[id];

		auto any_args = fill_args(std::forward<Args>(args)...);
		// Iterate this way to allow modification
		bool collect_garbage = false;
		for(size_t i = 0; i < container.size(); ++i)
		{
			const auto& info = container[i];
			if(info.sentinel.has_value() && info.sentinel.value().expired())
			{
				collect_garbage = true;
				continue;
			}

			info.function->invoke(any_args);
		}
		if(collect_garbage)
		{
			check_sentinels(container);
		}
	}

	void clear()
	{
		list_.clear();
	}

private:
	void sort(std::vector<info_t>& container)
	{
		std::sort(std::begin(container), std::end(container),
				  [](const info_t& info1, const info_t& info2) { return info1.priority > info2.priority; });
	}

	void check_sentinels(std::vector<info_t>& container)
	{
		// remove expired subscribers
		container.erase(std::remove_if(std::begin(container), std::end(container),
									   [](const info_t& info) {
										   return info.sentinel.has_value() &&
												  info.sentinel.value().expired();
									   }),
						std::end(container));
	}

	/// signal / slots
	std::unordered_map<id_t, std::vector<info_t>> list_;
};

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class functions_t
{
	struct info_t
	{
		/// Sentinel used for life tracking
		nonstd::optional<sentinel_t> sentinel;
		/// The function wrapper
		std::unique_ptr<base_function_wrapper> function;
	};

public:
	template <typename F>
	void bind(const id_t& id, F&& f)
	{
		info_t info;
		info.function = make_wrapper(std::forward<F>(f));
		list_[id] = std::move(info);
	}

	template <typename C, typename F>
	void bind(const id_t& id, C* const object_ptr, F&& f)
	{
		info_t info;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));
		list_[id] = std::move(info);
	}

	template <typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, F&& f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = make_wrapper(std::forward<F>(f));
		list_[id] = std::move(info);
	}

	template <typename C, typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, C* const object_ptr, F&& f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));
		list_[id] = std::move(info);
	}

	void unbind(const id_t& id)
	{
		list_.erase(id);
	}

	template <typename R, typename... Args,
			  typename std::enable_if<!std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args)
	{
		static_assert(!std::is_reference<R>::value, "unsupported return by reference (use return by value)");
		auto it = list_.find(id);
		assert((it != list_.end()) && "invoking a function without being binded to "
									  "any and expecting a result");
		const auto& info = it->second;

		// check if subscriber expired
		R res{};
		if(info.sentinel.has_value() && info.sentinel.value().expired())
		{
			list_.erase(it);
			assert(false && "invoking a function without being binded to any and "
							"expecting a result");
			return res;
		}

		if(info.function != nullptr)
		{
			auto result = info.function->invoke(fill_args(std::forward<Args>(args)...));
			bool can_cast = try_implicit_cast<R>(result, res);
			assert(can_cast && "cannot implicitly convert return type");
		}
		return res;
	}

	template <typename R = void, typename... Args,
			  typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args)
	{
		static_assert(!std::is_reference<R>::value, "unsupported return by reference (use return by value)");
		auto it = list_.find(id);
		assert((it != list_.end()) && "invoking a function without being binded to "
									  "any");
		const auto& info = it->second;

		// check if subscriber expired
		if(info.sentinel.has_value() && info.sentinel.value().expired())
		{
			list_.erase(it);
			assert(false && "invoking a function without being binded to any and "
							"expecting a result");
			return;
		}

		if(info.function != nullptr)
		{
			info.function->invoke(fill_args(std::forward<Args>(args)...));
		}
	}

	void clear()
	{
		list_.clear();
	}

private:
	/// signal / slots
	std::unordered_map<id_t, info_t> list_;
};

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
struct binder_t
{
	signals_t<id_t, sentinel_t> signals;
	functions_t<id_t, sentinel_t> functions;
};

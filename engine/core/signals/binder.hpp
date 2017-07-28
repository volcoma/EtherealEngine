#ifndef BINDER_HPP
#define BINDER_HPP

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
#include "../common/nonstd/function_traits.hpp"
#include "../common/nonstd/optional.hpp"
#include "../common/nonstd/sequence.hpp"
#include "../common/nonstd/type_traits.hpp"
#include "delegate.hpp"

// Used for type deduction ease
struct void_func
{
	using type = void_func;
};

// Used for type deduction ease
struct return_func
{
	using type = return_func;
};

template <typename T>
std::reference_wrapper<T> make_ref(T& val)
{
	return std::ref(val);
}

template <typename T>
std::reference_wrapper<T> make_ref(const T& val)
{
	return std::ref(const_cast<T&>(val));
}

// Create vector of any from variadic pack
template <typename... Args>
std::vector<nonstd::any> fill_args(Args&&... args)
{
	return {(std::is_pointer<Args>::value ? nonstd::any(args) : nonstd::any(make_ref(args)))...};
}

template <typename Arg, typename std::enable_if<!std::is_pointer<Arg>::value>::type* = nullptr>
nonstd::any create_arg()
{
	static typename std::decay<Arg>::type arg = {};
	return nonstd::any(std::ref(arg));
}
template <typename Arg, typename std::enable_if<std::is_pointer<Arg>::value>::type* = nullptr>
nonstd::any create_arg()
{
	static Arg arg = nullptr;
	return nonstd::any(arg);
}

template <typename target_t, typename std::enable_if<std::is_pointer<target_t>::value>::type* = nullptr>
bool check_arg_type(const nonstd::any& operand)
{
	return (operand.type() == rtti::type_id<target_t>());
}

template <typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
bool check_arg_type(const nonstd::any& operand)
{
	using decayed_t = typename std::decay<T>::type;
	using target_t = typename std::reference_wrapper<decayed_t>;
	return (operand.type() == rtti::type_id<target_t>());
}

template <typename target_t, typename std::enable_if<std::is_pointer<target_t>::value>::type* = nullptr>
target_t get_arg_type(const nonstd::any& operand)
{
	return nonstd::any_cast<target_t>(operand);
}

template <typename T, typename std::enable_if<!std::is_pointer<T>::value>::type* = nullptr>
T get_arg_type(const nonstd::any& operand)
{
	using decayed_t = typename std::decay<T>::type;
	using target_t = typename std::reference_wrapper<decayed_t>;
	return nonstd::any_cast<target_t>(operand).get();
}

template <typename Arg, typename std::enable_if<!std::is_reference<Arg>::value>::type* = nullptr>
nonstd::any create_return_value(Arg&& arg)
{
	return nonstd::any(arg);
}

template <typename Arg, typename std::enable_if<std::is_reference<Arg>::value>::type* = nullptr>
nonstd::any create_return_value(Arg&& arg)
{
	return nonstd::any(std::ref(arg));
}

template <typename target_t, typename std::enable_if<!std::is_reference<target_t>::value>::type* = nullptr>
bool check_return_type(const nonstd::any& operand)
{
	return (operand.type() == rtti::type_id<target_t>());
}

template <typename T, typename std::enable_if<std::is_reference<T>::value>::type* = nullptr>
bool check_return_type(const nonstd::any& operand)
{
	using decayed_t = typename std::decay<T>::type;
	using target_t = typename std::reference_wrapper<decayed_t>;
	return (operand.type() == rtti::type_id<target_t>());
}

template <typename target_t, typename std::enable_if<!std::is_reference<target_t>::value>::type* = nullptr>
target_t get_return_type(const nonstd::any& operand)
{
	return nonstd::any_cast<target_t>(operand);
}

template <typename T, typename std::enable_if<std::is_reference<T>::value>::type* = nullptr>
T get_return_type(const nonstd::any& operand)
{
	using decayed_t = typename std::decay<T>::type;
	using target_t = typename std::reference_wrapper<decayed_t>;
	return nonstd::any_cast<target_t>(operand).get();
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
class function_wrapper<F, nonstd::index_sequence<ArgCount...>> : public base_function_wrapper
{
	template <std::size_t... Arg_Idx>
	nonstd::any invoke_variadic_impl(nonstd::index_sequence<Arg_Idx...>,
									 const std::vector<nonstd::any>& arg_list) const
	{
		static const auto deduction_helper =
			typename std::conditional<std::is_same<typename nonstd::fn_result_of<F>, void>::value, void_func,
									  return_func>::type();
		// assert(nonstd::function_traits<F>::arity == arg_list.size() &&
		// "subscriber and invoker differ in
		// arguments count");
		return invoke(deduction_helper,
					  ((Arg_Idx < arg_list.size()) ? arg_list[Arg_Idx]
												   : create_arg<nonstd::param_types_t<F, Arg_Idx>>())...);
	}

	// Invoke void function, "const Args&...args" are of type "any"
	template <typename... Args>
	nonstd::any invoke(const void_func&, const Args&... args) const
	{
		const auto all_params_are_convertible =
			nonstd::check_all_true(check_arg_type<nonstd::param_types_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if(all_params_are_convertible)
		{
			_function(get_arg_type<nonstd::param_types_t<F, ArgCount>>(args)...);
		}

		return nonstd::any{};
	}

	// Invoke non void function, "const Args&...args" are of type "any"
	template <typename... Args>
	nonstd::any invoke(const return_func&, const Args&... args) const
	{
		const auto all_params_are_convertible =
			nonstd::check_all_true(check_arg_type<nonstd::param_types_t<F, ArgCount>>(args)...);
		assert(all_params_are_convertible && "cannot convert all the parameters");
		if(all_params_are_convertible)
		{
			return create_return_value(_function(get_arg_type<nonstd::param_types_t<F, ArgCount>>(args)...));
		}
		return nonstd::any{};
	}

public:
	using delegate_t = delegate<typename nonstd::function_traits<F>::function_type>;

	template <typename C>
	function_wrapper(C* const object_ptr, F&& f)
		: _function(object_ptr, f)
	{
	}

	function_wrapper(F&& f)
		: _function(f)
	{
	}

	virtual nonstd::any invoke(const std::vector<nonstd::any>& params) const
	{
		return invoke_variadic_impl(nonstd::make_index_sequence<nonstd::function_traits<F>::arity>(), params);
	}

	virtual bool owns(const nonstd::any& any_delegate) const
	{
		if(any_delegate.type() == rtti::type_id<delegate_t>())
		{
			return _function == nonstd::any_cast<delegate_t>(any_delegate);
		}
		return false;
	}

private:
	delegate_t _function;
};

template <typename F>
std::unique_ptr<base_function_wrapper> make_wrapper(F&& f)
{
	using arg_index_sequence = nonstd::make_index_sequence<nonstd::function_traits<F>::arity>;
	using wrapper = function_wrapper<F, arg_index_sequence>;
	return std::unique_ptr<wrapper>(new wrapper(std::forward<F>(f)));
}

template <typename C, typename F>
std::unique_ptr<base_function_wrapper> make_wrapper(C* const object_ptr, F&& f)
{
	using arg_index_sequence = nonstd::make_index_sequence<nonstd::function_traits<F>::arity>;
	using wrapper = function_wrapper<F, arg_index_sequence>;
	return std::unique_ptr<wrapper>(new wrapper(object_ptr, std::forward<F>(f)));
}

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class signals_t
{
	struct info_t
	{
		info_t() = default;

		info_t(info_t&& rhs)
			: priority(rhs.priority)
			, sentinel(rhs.sentinel)
			, function(rhs.function.release())
		{
		}

		info_t& operator=(info_t&& rhs)
		{
			priority = rhs.priority;
			sentinel = rhs.sentinel;
			function.reset(rhs.function.release());
			return *this;
		}

		~info_t() = default;
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

		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename C, typename F>
	void connect(const id_t& id, C* const object_ptr, F&& f, std::uint32_t priority = 0)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		info_t info;
		info.priority = priority;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));

		auto& container = _list[id];
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

		auto& container = _list[id];
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

		auto& container = _list[id];
		container.emplace_back(std::move(info));
		sort(container);
	}

	template <typename C, typename F>
	void disconnect(const id_t& id, C* const object_ptr, F&& f)
	{
		static_assert(std::is_same<void, typename nonstd::function_traits<F>::result_type>::value,
					  "signals cannot have a return type different from void");

		auto& container = _list[id];

		using arg_index_sequence = nonstd::make_index_sequence<nonstd::function_traits<F>::arity>;
		auto slot =
			typename function_wrapper<F, arg_index_sequence>::delegate_t(object_ptr, std::forward<F>(f));
		container.erase(std::remove_if(std::begin(container), std::end(container),
									   [&slot](const info_t& info) { return info.function->owns(slot); }),
						std::end(container));

		// If it was the last one, remove the whole container
		if(container.empty())
			_list.erase(id);
	}

	template <typename... Args>
	void emit(const id_t& id, Args&&... args) const
	{
		auto& container = _list[id];

		check_sentinels(container);

		auto any_args = fill_args(std::forward<Args>(args)...);
		// Iterate this way to allow modification
		for(size_t i = 0; i < container.size(); ++i)
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
	static void sort(std::vector<info_t>& container)
	{
		std::sort(std::begin(container), std::end(container),
				  [](const info_t& info1, const info_t& info2) { return info1.priority > info2.priority; });
	}

	static void check_sentinels(std::vector<info_t>& container)
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
	mutable std::unordered_map<id_t, std::vector<info_t>> _list;
};

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
class functions_t
{
	struct info_t
	{
		info_t() = default;

		info_t(info_t&& rhs)
			: sentinel(rhs.sentinel)
			, function(rhs.function.release())
		{
		}

		info_t& operator=(info_t&& rhs)
		{
			sentinel = rhs.sentinel;
			function.reset(rhs.function.release());
			return *this;
		}

		~info_t() = default;
		/// Sentinel used for life tracking
		nonstd::optional<sentinel_t> sentinel;
		/// The function wrapper
		std::unique_ptr<base_function_wrapper> function;
	};

public:
	template <typename F>
	void bind(const id_t& id, F f)
	{
		info_t info;
		info.function = make_wrapper(std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template <typename C, typename F>
	void bind(const id_t& id, C* const object_ptr, F f)
	{
		info_t info;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template <typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, F f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = make_wrapper(std::forward<F>(f));
		_list[id] = std::move(info);
	}

	template <typename C, typename F>
	void bind(const id_t& id, const sentinel_t& sentinel, C* const object_ptr, F f)
	{
		info_t info;
		info.sentinel = sentinel;
		info.function = make_wrapper(object_ptr, std::forward<F>(f));
		_list[id] = std::move(info);
	}

	void unbind(const id_t& id)
	{
		_list.erase(id);
	}

	template <typename R, typename... Args,
			  typename std::enable_if<!std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args) const
	{
		auto it = _list.find(id);
		assert((it != _list.end()) && "invoking a function without being binded to "
									  "any and expecting a result");
		const auto& info = it->second;

		// check if subscriber expired
		if(info.sentinel.has_value() && info.sentinel.value().expired())
		{
			_list.erase(it);
			assert(false && "invoking a function without being binded to any and "
							"expecting a result");
		}

		auto result = info.function->invoke(fill_args(std::forward<Args>(args)...));
		const auto can_cast = check_return_type<R>(result);
		assert(can_cast && "cannot implicitly convert return type");
		return get_return_type<R>(result);
	}

	template <typename R = void, typename... Args,
			  typename std::enable_if<std::is_same<R, void>::value>::type* = nullptr>
	R invoke(const id_t& id, Args&&... args) const
	{
		auto it = _list.find(id);
		if(it == _list.end())
			return;

		const auto& info = it->second;

		// check if subscriber expired
		if(info.sentinel.has_value() && info.sentinel.value().expired())
		{
			_list.erase(it);
			assert(false && "invoking a function without being binded to any");
			return;
		}

		info.function->invoke(fill_args(std::forward<Args>(args)...));
	}

	void clear()
	{
		_list.clear();
	}

private:
	/// signal / slots
	mutable std::unordered_map<id_t, info_t> _list;
};

template <typename id_t = std::string, typename sentinel_t = std::weak_ptr<void>>
struct binder
{
	signals_t<id_t, sentinel_t> signals;
	functions_t<id_t, sentinel_t> functions;
};

#endif

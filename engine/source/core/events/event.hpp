#pragma once
#ifndef EVENT_HPP
#define EVENT_HPP

#include <vector>
#include <utility>

#include "signal.hpp"
#include "delegate.hpp"

template <class Signature>
struct event
{
private:
	typedef event<Signature> this_type;
public:
	typedef delegate<Signature> delegate_type;
	typedef signal<delegate_type> signal_type;


	/// Connects a slot to the signal
	/// \param args The arguments you wish to construct the slot with to connect to the signal
	template <typename... Args>
	void connect(Args&&... args)
	{
		_signal.connect(std::forward<Args>(args)...);
	}

	/// Disconnects a slot from the signal
	/// \param args The arguments you wish to construct a slot with
	template <typename... Args>
	void disconnect(Args&&... args)
	{
		_signal.disconnect(std::forward<Args>(args)...);
	}

	/// Emits events you wish to send to call-backs
	/// \param args The arguments to emit to the slots connected to the signal
	/// \note
	/// This is equvialent to emit.
	template <class ...Args>
	void operator()(Args&&... args) const
	{
		_signal.emit(std::forward<Args>(args)...);
	}

	template <typename... Args>
	event& operator+=(Args&&... args)
	{
		connect(std::forward<Args>(args)...);
		return *this;
	}

	template <typename... Args>
	event& operator-=(Args&&... args)
	{
		disconnect(std::forward<Args>(args)...);
		return *this;
	}
	// comparision operators for sorting and comparing
	bool operator==(const this_type& event) const
	{
		return _signal == event._signal;
	}

	bool operator!=(const this_type& event) const
	{
		return !operator==(event);
	}

private:

	/// The internal impl
	signal_type _signal;
};


#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

template <typename F>
struct function_traits : public function_traits<decltype(&F::operator())>
{};

template <typename T, typename R, typename... Args>
struct function_traits<R(T::*)(Args...) const>
{
	typedef R(*pointer)(Args...);
	typedef R return_type;
	static constexpr std::size_t arg_count = sizeof...(Args);
	typedef std::tuple<Args...> args_tuple;
	typedef const std::function<R(Args...)> function;
};

struct function_wrapper
{
	virtual ~function_wrapper() {}
	virtual const void* get_ptr() const= 0;
};

template<typename F>
class function_wrapper_t : public function_wrapper
{
public:
	function_wrapper_t(F&& f) : _function(f) {}
	~function_wrapper_t() {}
	const void* get_ptr() const { return &_function; }

private:
	typename function_traits<F>::function _function;
};

template <typename F>
std::unique_ptr<function_wrapper> create_wrapper(F&& f)
{
	return std::unique_ptr<function_wrapper_t<typename F>>(new function_wrapper_t<typename F>(std::forward<F>(f)));
}

class event_dispatcher
{
public:
	template<typename F>
	void connect(const std::string& name, F&& f)
	{
		static_assert(std::is_same<void, typename function_traits<F>::return_type>::value,
			"Signals cannot have a return type different from void");

		_list[name].emplace_back(create_wrapper(std::forward<F>(f)));
	}

	template<typename ... Args>
	void dispatch(const std::string& name, Args... args)
	{
		auto& funcs = _list[name];

		for (auto& func : funcs)
		{
			auto& f = *reinterpret_cast<const std::function<void(Args...)>*>(func->get_ptr());
			f(std::forward<Args>(args) ...);
		}
	}
private:
	std::unordered_map<std::string, std::vector<std::unique_ptr<function_wrapper>>> _list;
};

#endif // EVENT_HPP

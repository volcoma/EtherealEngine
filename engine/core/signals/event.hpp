#pragma once
#ifndef EVENT_HPP
#define EVENT_HPP

#include "../common/nonstd/slot_map.hpp"
#include "delegate.hpp"
#include <algorithm>
#include <utility>
#include <vector>

template <typename T>
class event;

template <typename... Args>
class event<void(Args...)>
{
public:
	using slot_type = delegate<void(Args...)>;
	using slot_container = nonstd::slot_map<slot_type>;
	using slot_key = typename slot_container::key_type;

	template <class C>
	slot_key connect(C* const object_ptr, void (C::*const method_ptr)(Args...))
	{
		return slots_.emplace(object_ptr, method_ptr);
	}

	template <class C>
	slot_key connect(C* const object_ptr, void (C::*const method_ptr)(Args...) const)
	{
		return slots_.emplace(object_ptr, method_ptr);
	}

	template <typename T, typename = typename std::enable_if<
							  !std::is_same<event, typename std::decay<T>::type>::value>::type>
	slot_key connect(T&& f)
	{
		return slots_.emplace(std::forward<T>(f));
	}

	template <class C>
	void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...))
	{
		slot_type slot(object_ptr, method_ptr);
		disconnect_by_value(slot);
	}

	template <class C>
	void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...) const)
	{
		slot_type slot(object_ptr, method_ptr);
		disconnect_by_value(slot);
	}

	template <typename T, typename = typename std::enable_if<
							  !std::is_same<event, typename std::decay<T>::type>::value &&
							  !std::is_same<slot_key, typename std::decay<T>::type>::value>::type>
	void disconnect(T&& f)
	{
		slot_type slot(std::forward<T>(f));
		disconnect_by_value(slot);
	}

	void disconnect(const slot_key& key) noexcept
	{
		disconnect_by_key(key);
	}

	/// Emits the events you wish to send to the call-backs
	/// \param args The arguments to emit to the slots connected to the signal
	void emit(Args... args) const
	{
		for(auto& slot : slots_)
		{
			slot(std::forward<Args>(args)...);
		}
	}

	/// Emits events you wish to send to call-backs
	/// \param args The arguments to emit to the slots connected to the signal
	/// \note
	/// This is equvialent to emit.
	void operator()(Args... args) const
	{
		emit(std::forward<Args>(args)...);
	}

	// comparision operators for sorting and comparing

	bool operator==(const event& s) const
	{
		return slots_ == s.slots_;
	}

	bool operator!=(const event& s) const
	{
		return !operator==(s);
	}

private:
	void disconnect_by_value(const slot_type& slot)
	{
		auto it = std::find_if(std::begin(slots_), std::end(slots_),
							   [&slot](const slot_type& other) { return slot == other; });

		if(it != std::end(slots_))
		{
			slots_.erase(it);
		}
	}

	void disconnect_by_key(const slot_key& key) noexcept
	{
		slots_.erase(key);
	}
	/// The slots connected to the signal
	slot_container slots_;
};

#endif // EVENT_HPP

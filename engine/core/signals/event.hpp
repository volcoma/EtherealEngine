#pragma once
#ifndef EVENT_HPP
#define EVENT_HPP

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

	template <class C, typename = typename ::std::enable_if<::std::is_class<C>::value, C>::type>
	void connect(C const* const o) noexcept
	{
		_slots.emplace_back(slot_type(o));
	}

	template <class C, typename = typename ::std::enable_if<::std::is_class<C>::value, C>::type>
	void connect(C const& o) noexcept
	{
		_slots.emplace_back(slot_type(o));
	}

	template <class C>
	void connect(C* const object_ptr, void (C::*const method_ptr)(Args...))
	{
		_slots.emplace_back(slot_type(object_ptr, method_ptr));
	}

	template <class C>
	void connect(C* const object_ptr, void (C::*const method_ptr)(Args...) const)
	{
		_slots.emplace_back(slot_type(object_ptr, method_ptr));
	}

	template <class C>
	void connect(C& object, void (C::*const method_ptr)(Args...))
	{
		_slots.emplace_back(slot_type(object, method_ptr));
	}

	template <class C>
	void connect(C const& object, void (C::*const method_ptr)(Args...) const)
	{
		_slots.emplace_back(slot_type(object, method_ptr));
	}

	template <typename T, typename = typename ::std::enable_if<
							  !::std::is_same<event, typename ::std::decay<T>::type>::value>::type>
	void connect(T&& f)
	{
		_slots.emplace_back(slot_type(std::forward<T>(f)));
	}

	template <class C, typename = typename ::std::enable_if<::std::is_class<C>::value, C>::type>
	void disconnect(C const* const o) noexcept
	{
		slot_type slot(o);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <class C, typename = typename ::std::enable_if<::std::is_class<C>::value, C>::type>
	void disconnect(C const& o) noexcept
	{
		slot_type slot(o);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <class C>
	void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...))
	{
		slot_type slot(object_ptr, method_ptr);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <class C>
	void disconnect(C* const object_ptr, void (C::*const method_ptr)(Args...) const)
	{
		slot_type slot(object_ptr, method_ptr);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <class C>
	void disconnect(C& object, void (C::*const method_ptr)(Args...))
	{
		slot_type slot(object, method_ptr);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <class C>
	void disconnect(C const& object, void (C::*const method_ptr)(Args...) const)
	{
		slot_type slot(object, method_ptr);
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	template <typename T, typename = typename ::std::enable_if<
							  !::std::is_same<event, typename ::std::decay<T>::type>::value>::type>
	void disconnect(T&& f)
	{
		slot_type slot(std::forward<T>(f));
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
									[&slot](const slot_type& other) { return slot == other; }),
					 std::end(_slots));
	}

	/// Emits the events you wish to send to the call-backs
	/// \param args The arguments to emit to the slots connected to the signal
	void emit(Args... args) const
	{
		for(auto& slot : _slots)
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
		return _slots == s._slots;
	}

	bool operator!=(const event& s) const
	{
		return !operator==(s);
	}

private:
	/// defines an array of slots
	using slot_array = std::vector<slot_type>;

	/// The slots connected to the signal
	slot_array _slots;
};

#endif // EVENT_HPP

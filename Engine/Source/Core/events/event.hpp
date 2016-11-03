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
	void addListener(Args&&... args)
	{
		_signal.connect(std::forward<Args>(args)...);
	}

	/// Disconnects a slot from the signal
	/// \param args The arguments you wish to construct a slot with
	template <typename... Args>
	void removeListener(Args&&... args)
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
		addListener(std::forward<Args>(args)...);
		return *this;
	}

	template <typename... Args>
	event& operator-=(Args&&... args)
	{
		removeListener(std::forward<Args>(args)...);
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

#endif // EVENT_HPP

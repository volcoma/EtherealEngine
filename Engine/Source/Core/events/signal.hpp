#pragma once
#ifndef SIGNAL_HPP
#define SIGNAL_HPP

#include <vector>
#include <utility>
#include <mutex>
#include <algorithm>

template <class Slot>
struct signal
{
private:
	
	typedef signal<Slot> this_type;
	
public:
	
	typedef Slot slot_type;
	
	/// Connects a slot to the signal
	/// \param args The arguments you wish to construct the slot with to connect to the signal
	template <typename... Args>
	void connect(Args&&... args)
	{
		_slots.emplace_back(std::forward<Args>(args)...);
	}
	
	/// Disconnects a slot from the signal
	/// \param args The arguments you wish to construct a slot with
	template <typename... Args>
	void disconnect(Args&&... args)
	{
		slot_type slot(slot_type(std::forward<Args>(args)...));
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
			[&slot](Slot other) { return slot == other; }
		), std::end(_slots));
	}

	/// Connects a slot to the signal
	/// \param args The arguments you wish to construct the slot with to connect to the signal
	template <typename... Args>
	void addListener(Args&&... args)
	{
		_slots.emplace_back(std::forward<Args>(args)...);
	}

	/// Disconnects a slot from the signal
	/// \param args The arguments you wish to construct a slot with
	template <typename... Args>
	void removeListener(Args&&... args)
	{
		slot_type slot(slot_type(std::forward<Args>(args)...));
		_slots.erase(std::remove_if(std::begin(_slots), std::end(_slots),
			[&slot](Slot other) { return slot == other; }
		), std::end(_slots));
	}
	
	/// Emits the events you wish to send to the call-backs
	/// \param args The arguments to emit to the slots connected to the signal
	template <class ...Args>
	void emit(Args&&... args) const
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
	template <class ...Args>
	void operator()(Args&&... args) const
	{
		emit(std::forward<Args>(args)...);
	}
	
	// comparision operators for sorting and comparing
	
	bool operator==(const this_type& signal) const
	{ return _slots == signal._slots; }
	
	bool operator!=(const this_type& signal) const
	{ return !operator==(signal); }
	
private:
	
	/// defines an array of slots
	typedef std::vector<slot_type> slot_array;
	
	/// The slots connected to the signal
	slot_array _slots;
};

#endif // SIGNAL_HPP

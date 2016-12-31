#pragma once

#include "core/logging/logging.h"
#include <core/console/console.h>
#include <string>
#include <deque>
class ConsoleLog : public logging::sinks::base_sink<std::mutex>, public Console
{
public:
	using ItemContainer = std::deque<std::pair<std::string, logging::level::level_enum>>;

	//-----------------------------------------------------------------------------
	//  Name : _sink_it ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void _sink_it(const logging::details::log_msg& msg) override;

	//-----------------------------------------------------------------------------
	//  Name : flush ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void flush() override;

	//-----------------------------------------------------------------------------
	//  Name : getItems ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ItemContainer getItems();

	//-----------------------------------------------------------------------------
	//  Name : clearLog ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void clearLog();
	
	//-----------------------------------------------------------------------------
	//  Name : getPendingEntries ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline int getPendingEntries() const { return mPendingEntries; }

	//-----------------------------------------------------------------------------
	//  Name : setPendingEntries ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setPendingEntries(bool val) { mPendingEntries = val; }
private:
	///
	ItemContainer mItems;
	///
	int mPendingEntries = 0;
	///
	const std::size_t mMaxSize = 50;
};
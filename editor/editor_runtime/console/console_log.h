#pragma once

#include "core/logging/logging.h"
#include <array>
#include <atomic>
#include <core/console/console.h>
#include <deque>
#include <string>

class console_log : public logging::sinks::base_sink<std::mutex>, public console
{
public:
	using entries_t = std::deque<std::pair<std::string, logging::level::level_enum>>;

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
	void _flush() override;

	//-----------------------------------------------------------------------------
	//  Name : get_items ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	entries_t get_items();

	//-----------------------------------------------------------------------------
	//  Name : clearLog ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear_log();

	//-----------------------------------------------------------------------------
	//  Name : get_pending_entries ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool has_new_entries() const
	{
		return _has_new_entries;
	}

	const std::array<float, 4>& get_level_colorization(logging::level::level_enum level);
	//-----------------------------------------------------------------------------
	//  Name : set_pending_entries ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_has_new_entries(bool val)
	{
		_has_new_entries = val;
	}

private:
	std::recursive_mutex _entries_mutex;
	///
	entries_t _entries;
	///
	std::atomic<bool> _has_new_entries = {false};
	///
	static const std::size_t _max_size = 150;
};

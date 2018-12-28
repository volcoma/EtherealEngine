#pragma once

#include <core/common/nonstd/ring_buffer.hpp>
#include <core/console/console.h>
#include <core/logging/logging.h>

#include <array>
#include <atomic>
#include <string>

class console_log : public logging::sinks::base_sink<std::mutex>, public console
{
public:
	template <typename T>
	using ring_buffer = nonstd::stack_ringbuffer<T, 150>;
	using entries_t = ring_buffer<std::pair<std::string, logging::level::level_enum>>;

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
		return has_new_entries_;
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
		has_new_entries_ = val;
	}

private:
	std::recursive_mutex entries_mutex_;
	///
	entries_t entries_;
	///
	std::atomic<bool> has_new_entries_ = {false};
};

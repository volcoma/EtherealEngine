#include "console_log.h"
#include <map>

void console_log::_sink_it(const logging::details::log_msg& msg)
{
	{
		std::lock_guard<std::recursive_mutex> lock(entries_mutex_);
		entries_.push_back({msg.formatted.c_str(), msg.level});
	}
	has_new_entries_ = true;
}

void console_log::_flush()
{
}

console_log::entries_t console_log::get_items()
{
	entries_t items_copy;
	{
		std::lock_guard<std::recursive_mutex> lock(entries_mutex_);
		items_copy = entries_;
	}
	return items_copy;
}

void console_log::clear_log()
{
	{
		std::lock_guard<std::recursive_mutex> lock(entries_mutex_);
		entries_.clear();
	}
	has_new_entries_ = false;
}

const std::array<float, 4>& console_log::get_level_colorization(logging::level::level_enum level)
{
	static std::map<logging::level::level_enum, std::array<float, 4>> colorization_mappings{
		{logging::level::trace, std::array<float, 4>{{1.0f, 1.0f, 1.0f, 1.0f}}},
		{logging::level::debug, {{1.0f, 1.0f, 1.0f, 1.0f}}},
		{logging::level::info, {{1.0f, 1.0f, 1.0f, 1.0f}}},
		{logging::level::warn, {{1.0f, 0.494f, 0.0f, 1.0f}}},
		{logging::level::err, {{1.0f, 0.0f, 0.0f, 1.0f}}},
		{logging::level::critical, {{1.0f, 1.0f, 1.0f, 1.0f}}},
		{logging::level::off, {{1.0f, 1.0f, 1.0f, 1.0f}}},
	};

	return colorization_mappings[level];
}

#include "console_log.h"


void console_log::_sink_it(const logging::details::log_msg& msg)
{
	_entries.push_back({ msg.formatted.c_str(), msg.level });
	if (_entries.size() > _max_size)
		_entries.pop_front();
	flush();
	++_pending;
}

void console_log::flush()
{

}

console_log::entries_t console_log::get_items()
{
	entries_t itemsCopy;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		itemsCopy = _entries;
	}
	return itemsCopy;
}

void console_log::clearLog()
{
	_entries = entries_t();
	_pending = 0;
}
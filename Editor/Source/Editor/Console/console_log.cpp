#include "console_log.h"


void ConsoleLog::_sink_it(const logging::details::log_msg& msg)
{
	mItems.push_back({ msg.formatted.c_str(), msg.level });
	if (mItems.size() > mMaxSize)
		mItems.pop_front();
	flush();
	++mPendingEntries;
}

void ConsoleLog::flush()
{

}

ConsoleLog::ItemContainer ConsoleLog::getItems()
{
	ItemContainer itemsCopy;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		itemsCopy = mItems;
	}
	return itemsCopy;
}

void ConsoleLog::clearLog()
{
	mItems = ItemContainer();
	mPendingEntries = 0;
}
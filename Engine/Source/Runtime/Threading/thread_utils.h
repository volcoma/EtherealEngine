#pragma once

#include <thread>
#include <string>

namespace thread_utils
{
	void setThreadName(std::thread* thread, const std::string& threadName);
}

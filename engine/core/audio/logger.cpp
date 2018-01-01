#include "logger.h"

namespace audio
{
namespace priv
{
logger_t& get_info_logger()
{
	static logger_t logger;
	return logger;
}
logger_t& get_error_logger()
{
	static logger_t logger;
	return logger;
}
}

void log_info(const std::string& msg)
{
	auto logger = priv::get_info_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_info_logger(std::function<void(const std::string&)> logger)
{
	priv::get_info_logger() = std::move(logger);
}

void log_error(const std::string& msg)
{
	auto logger = priv::get_error_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_error_logger(std::function<void(const std::string&)> logger)
{
	priv::get_error_logger() = std::move(logger);
}
}

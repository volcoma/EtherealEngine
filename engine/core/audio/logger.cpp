#include "logger.h"

namespace audio
{
namespace priv
{
logger_t& get_logger()
{
	static logger_t logger;
	return logger;
}


}

void log(const std::string& msg)
{
	auto logger = priv::get_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_logger(std::function<void(const std::string&)> logger)
{
	priv::get_logger() = std::move(logger);
}
}

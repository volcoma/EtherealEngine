#include "serialization.h"

namespace serialization
{
static std::function<void(const std::string& log_msg)> warning_logger;
void set_warning_logger(std::function<void(const std::string& log_msg)> logger)
{
	warning_logger = logger;
}
void log_warning(const std::string& log_msg)
{
	if(warning_logger)
		warning_logger(log_msg);
}
}

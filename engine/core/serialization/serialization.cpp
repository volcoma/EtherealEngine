#include "serialization.h"

namespace serialization
{
static std::function<void(const std::string&)> warning_logger;
void set_warning_logger(const std::function<void(const std::string&)>& logger)
{
	warning_logger = logger;
}
void log_warning(const std::string& log_msg)
{
	if(warning_logger)
		warning_logger(log_msg);
}
}

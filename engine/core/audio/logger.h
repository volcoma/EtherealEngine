#pragma once
#include <functional>
#include <string>
#include <vector>

namespace audio
{
using logger_t = std::function<void(const std::string&)>;

bool init();
void deinit();

void set_info_logger(logger_t logger);
void set_error_logger(logger_t logger);

void log_info(const std::string& msg);
void log_error(const std::string& msg);

}

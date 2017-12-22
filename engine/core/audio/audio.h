#pragma once
#include <functional>
#include <string>
#include <vector>

namespace audio
{
using logger_t = std::function<void(const std::string&)>;

bool init();
void deinit();
std::string info();
std::vector<std::string> enumerate();

void set_logger(logger_t logger);
void log(const std::string& msg);
}

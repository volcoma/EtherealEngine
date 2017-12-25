#pragma once
#include <functional>
#include <string>
#include <vector>

namespace audio
{
using logger_t = std::function<void(const std::string&)>;

//-----------------------------------------------------------------------------
//  Name : set_info_logger ()
/// <summary>
/// Sets and extern info logging function for the library to use.
/// </summary>
//-----------------------------------------------------------------------------
void set_info_logger(logger_t logger);

//-----------------------------------------------------------------------------
//  Name : set_error_logger ()
/// <summary>
/// Sets and extern error logging function for the library to use.
/// </summary>
//-----------------------------------------------------------------------------
void set_error_logger(logger_t logger);

//-----------------------------------------------------------------------------
//  Name : log_info ()
/// <summary>
/// Used internally by the library to log informational messages.
/// </summary>
//-----------------------------------------------------------------------------
void log_info(const std::string& msg);

//-----------------------------------------------------------------------------
//  Name : log_error ()
/// <summary>
/// Used internally by the library to log error messages.
/// </summary>
//-----------------------------------------------------------------------------
void log_error(const std::string& msg);
}

#pragma once

#include "spdlog/spdlog.h"

#include "../common/platform_config.h"

#if $on($windows) 
#if $on($msvc)
#include "spdlog/sinks/msvc_sink.h"
namespace spdlog
{
namespace sinks
{
using platform_sink_mt = msvc_sink_mt;
using platform_sink_st = msvc_sink_st;
}
}
#else
#include "spdlog/sinks/wincolor_sink.h"
namespace spdlog
{
namespace sinks
{
using platform_sink_mt = wincolor_stdout_sink_mt;
using platform_sink_st = wincolor_stdout_sink_st;
}
}
#endif
#elif $on($linux) || $on($apple)
#include "spdlog/sinks/stdout_sinks.h"
namespace spdlog
{
namespace sinks
{
using platform_sink_mt = stdout_sink_mt;
using platform_sink_st = stdout_sink_st;
}
}
#elif $on($android)
#include "spdlog/sinks/android_sink.h"
namespace spdlog
{
namespace sinks
{
using platform_sink_mt = android_sink_mt;
using platform_sink_st = android_sink_st;
}
}
#else
#include "spdlog/sinks/stdout_sinks.h"
namespace spdlog
{
namespace sinks
{
using platform_sink_mt = stdout_sink_mt;
using platform_sink_st = stdout_sink_st;
}
}
#endif
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/file_sinks.h"

namespace logging
{
using namespace spdlog;
inline std::shared_ptr<sinks::dist_sink_mt> get_mutable_logging_container()
{
	static auto sink = std::make_shared<sinks::dist_sink_mt>();
	return sink;
}
#define APPLOG "Log"
#define APPLOG_INFO(...) spdlog::get("Log")->info(__VA_ARGS__)
#define APPLOG_TRACE(...) spdlog::get("Log")->trace(__VA_ARGS__)
#define APPLOG_ERROR(...) spdlog::get("Log")->error(__VA_ARGS__)
#define APPLOG_WARNING(...) spdlog::get("Log")->warn(__VA_ARGS__)
#define APPLOG_NOTICE(...) spdlog::get("Log")->notice(__VA_ARGS__)
#define APPLOG_SEPARATOR() APPLOG_INFO("-----------------------------")
}

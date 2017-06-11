#pragma  once

#include "spdlog/spdlog.h"
#include "spdlog/details/null_mutex.h"

#include "../common/platform_config.h"

#if $on($windows) && $on($msvc)
#include "spdlog/sinks/msvc_sink.h"
namespace spdlog
{
	namespace sinks
	{
		using platform_sink_mt = msvc_sink_mt;
		using platform_sink_st = msvc_sink_st;
	}
}
#elif $on($linux) || $on($apple)
#include "spdlog/sinks/syslog_sink.h"
namespace spdlog
{
	namespace sinks
	{
		using platform_sink_mt = syslog_sink;
		using platform_sink_st = syslog_sink;
	}
}
#elif $on($android)
#include "spdlog/sinks/syslog_sink.h"
namespace spdlog
{
	namespace sinks
	{
		using platform_sink_mt = android_sink_mt;
		using platform_sink_st = android_sink_st;
	}
}
#else
#include "spdlog/sinks/syslog_sink.h"
namespace spdlog
{
    namespace sinks
    {
        using platform_sink_mt = stdout_sink_mt;
        using platform_sink_st = stdout_sink_st;
    }
}
#endif
#include "spdlog/sinks/file_sinks.h"


namespace logging
{
	using namespace spdlog;

#define APPLOG "Log"
#define APPLOG_INFO(...) spdlog::get("Log")->info().write(__VA_ARGS__)
#define APPLOG_TRACE(...) spdlog::get("Log")->trace().write(__VA_ARGS__)
#define APPLOG_ERROR(...) spdlog::get("Log")->error().write(__VA_ARGS__)
#define APPLOG_WARNING(...) spdlog::get("Log")->warn().write(__VA_ARGS__)
#define APPLOG_NOTICE(...) spdlog::get("Log")->notice().write(__VA_ARGS__)

}

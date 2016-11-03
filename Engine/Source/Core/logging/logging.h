#pragma  once

#include "spdlog/spdlog.h"
#include "spdlog/details/null_mutex.h"

#include "../platform_config.h"

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
#endif
#include "spdlog/sinks/file_sinks.h"


namespace logging
{
	using namespace spdlog;
}
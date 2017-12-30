#pragma once

#include "platform_config.h"

#include <chrono>
#include <thread>

#if $on($windows) && $on($mingw)
#include <winsock.h>
namespace platform
{
namespace this_thread
{
template <typename rep, typename period>
inline void sleep_for(const std::chrono::duration<rep, period>& rtime)
{
	if(rtime <= rtime.zero())
		return;
	auto secs = std::chrono::duration_cast<std::chrono::seconds>(rtime);
	auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(rtime - secs);

	struct timeval tv;
	tv.tv_sec = secs.count();
	tv.tv_usec = usecs.count();
	select(0, NULL, NULL, NULL, &tv);
}
}
}
#else
namespace platform
{
namespace this_thread
{
template <typename rep, typename period>
inline void sleep_for(const std::chrono::duration<rep, period>& rtime)
{
    std::this_thread::sleep_for(rtime);
}
}
}

#endif

// An attempt at making a wrapper to deal with many Linuxes as well as Windows. Please edit as needed.
#if $on($windows) && $on($msvc)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace platform
{
#pragma pack(push, 8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType;	 // Must be 0x1000.
	LPCSTR szName;	// Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags;	// Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

inline void set_thread_name(DWORD dwThreadID, const char* threadName)
{
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = threadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	static const DWORD MS_VC_EXCEPTION = 0x406D1388;

	// Push an exception handler to ignore all following exceptions
#pragma warning(push)
#pragma warning(disable : 6320 6322)
	__try
	{
		RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
#pragma warning(pop)
}

inline void set_thread_name(std::thread& thread, const char* threadName)
{
	DWORD threadId = ::GetThreadId(reinterpret_cast<HANDLE>(thread.native_handle()));
	set_thread_name(threadId, threadName);
}
}
#else
namespace platform
{
inline void set_thread_name(std::thread& thread, const char* threadName)
{
	pthread_setname_np(thread.native_handle(), threadName);
}
}
#endif

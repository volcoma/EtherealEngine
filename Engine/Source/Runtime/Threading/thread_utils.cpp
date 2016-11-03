#include "thread_utils.h"

#include "Core/platform_config.h"

#if $on($windows)
#	include <windows.h>
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)
#else
#	include <pthread.h>
#endif
namespace thread_utils
{
void setThreadName(std::thread* thread, const std::string& threadName)
{
	$windows(
		DWORD dwThreadID = ::GetThreadId( static_cast<HANDLE>(thread->native_handle() ) );
		const DWORD MS_VC_EXCEPTION = 0x406D1388;

		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = threadName.c_str();
		info.dwThreadID = dwThreadID;
		info.dwFlags = 0;

		__try
		{
			RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	)
	$welse(
		pthread_setname_np(thread->native_handle(), threadName.c_str());
	)
}
}
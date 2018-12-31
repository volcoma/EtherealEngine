#include "filesystem_os.h"
#include "../../common/platform/config.hpp"
#include <array>
#include <cstdlib>

namespace fs
{
static path executable_path_fallback(const char* argv0)
{
	if(nullptr == argv0 || 0 == argv0[0])
	{
		return "";
	}
	fs::error_code err;
	path full_path(absolute(path(std::string(argv0)), err));
	return full_path;
}
}
#if ETH_ON(ETH_PLATFORM_WINDOWS)
#include <Windows.h>
namespace fs
{
path executable_path(const char* argv0)
{
	std::array<char, 1024> buf;
	buf.fill(0);
	DWORD ret = GetModuleFileNameA(nullptr, buf.data(), DWORD(buf.size()));
	if(ret == 0 || std::size_t(ret) == buf.size())
	{
		return executable_path_fallback(argv0);
	}
	return path(std::string(buf.data()));
}
void show_in_graphical_env(const path& _path)
{
	ShellExecuteA(nullptr, nullptr, _path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
}
#elif ETH_ON(ETH_PLATFORM_APPLE)
#include <mach-o/dyld.h>
namespace fs
{
path executable_path(const char* argv0)
{
	std::array<char, 1024> buf;
	buf.fill(0);
	uint32_t size = buf.size();
	int ret = _NSGetExecutablePath(buf.data(), &size);
	if(0 != ret)
	{
		return executable_path_fallback(argv0);
	}
	fs::error_code err;
	path full_path(absolute(fs::absolute(path(std::string(buf.data()))), err));
	return full_path;
}
void show_in_graphical_env(const path& _path)
{
}
}
#elif ETH_ON(ETH_PLATFORM_LINUX)

#include <unistd.h>
namespace fs
{
path executable_path(const char* argv0)
{
	std::array<char, 1024> buf;
	buf.fill(0);

	ssize_t size = readlink("/proc/self/exe", buf.data(), buf.size());
	if(size == 0 || size == sizeof(buf))
	{
		return executable_path_fallback(argv0);
	}
	std::string p(buf.data(), size);
	fs::error_code err;
	path full_path(absolute(fs::absolute(path(p)), err));
	return full_path;
}
void show_in_graphical_env(const path& _path)
{
	static std::string cmd = "xdg-open";
	static std::string space = " ";
	const std::string cmd_args = "'" + _path.string() + "'";
	const std::string whole_command = cmd + space + cmd_args;
	auto result = std::system(whole_command.c_str());
	(void)result;
}
}
#else
namespace fs
{
path executable_path(const char* argv0)
{
	return executable_path_fallback(argv0);
}

void show_in_graphical_env(const path& _path)
{
}
}
#endif

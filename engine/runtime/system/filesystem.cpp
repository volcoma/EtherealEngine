#include "filesystem.h"
#include "core/common/string.h"
#include "core/platform_config.h"

namespace fs
{

	bool add_path_protocol(const path& protocol, const path& dir)
	{
		// Protocol matching is case insensitive, convert to lower case
		auto protocol_lower = string_utils::to_lower(protocol.string());

		auto& protocols = get_path_protocols();
		// Add to the list
		protocols[protocol_lower] = dir.string();

		// Success!
		return true;
	}

	protocols_t& get_path_protocols()
	{
		static protocols_t mProtocols;
		return mProtocols;
	}

	byte_array_t read_stream(std::istream & stream)
	{
		// Open the stream
		byte_array_t read_memory;
		if (stream) {
			// get length of file:
			stream.seekg(0, stream.end);
			size_t length = static_cast<std::size_t>(stream.tellg());
			stream.seekg(0, stream.beg);

			read_memory.resize(length, '\0'); // reserve space
			char* begin = (char*)&*read_memory.begin();

			stream.read(begin, length);

			stream.clear();
			stream.seekg(0);
		}
		// Done
		return read_memory;
	}

	path resolve_protocol(const path& _path)
	{
		const auto root = _path.root_name().string();
		const auto relativePath = _path.relative_path();
		// Matching path protocol in our list?
		auto& protocols = get_path_protocols();

		auto it = protocols.find(root);

		if (it == std::end(protocols))
			return path{};

		const auto resolvedPath = it->second;
		auto result = resolvedPath / relativePath;
		return result;
	}

}

#include <stdio.h>
#include <stdlib.h>

namespace fs
{
	path executable_path_fallback(const char *argv0)
	{
		if (0 == argv0 || 0 == argv0[0])
		{
			return "";
		}
        fs::error_code err;
		path full_path(
			system_complete(
                path(std::string(argv0)), err));
		return full_path;
	}
}
#if $on($windows)
#include <Windows.h>
namespace fs
{
	path executable_path(const char *argv0)
	{
		char buf[1024] = { 0 };
		DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
		if (ret == 0 || ret == sizeof(buf))
		{
			return executable_path_fallback(argv0);
		}
		return path(std::string(buf));
	}
	void show_in_graphical_env(const path& _path)
	{
		ShellExecuteA(NULL, NULL, _path.string().c_str(), NULL, NULL, SW_SHOWNORMAL);
	}
}
#elif $on($apple)
#include <mach-o/dyld.h>
namespace fs
{
	path executable_path(const char *argv0)
	{
		char buf[1024] = { 0 };
		uint32_t size = sizeof(buf);
		int ret = _NSGetExecutablePath(buf, &size);
		if (0 != ret)
		{
			return executable_path_fallback(argv0);
		}
        fs::error_code err;
		path full_path(
			system_complete(
                path(std::string(buf)).normalize(), err));
		return full_path;
	}
	void show_in_graphical_env(const path& _path)
	{

	}
}
#elif $on($linux)

#include <unistd.h>
namespace fs
{
	path executable_path(const char *argv0)
	{
		char buf[1024] = { 0 };
		ssize_t size = readlink("/proc/self/exe", buf, sizeof(buf));
		if (size == 0 || size == sizeof(buf))
		{
			return executable_path_fallback(argv0);
		}
		std::string path(buf, size);
        fs::error_code err;
		path full_path(
			system_complete(
                path(path).normalize(), err));
		return full_path;
	}
	void show_in_graphical_env(const path& _path)
	{

	}
}
#else
namespace fs
{
	path executable_path(const char *argv0)
	{
		return executable_path_fallback(argv0);
	}

	void show_in_graphical_env(const path& _path)
	{

	}

}
#endif

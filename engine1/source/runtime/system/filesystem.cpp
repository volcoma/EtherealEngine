#include "filesystem.h"
#include "core/common/string_utils.h"
#include "core/platform_config.h"

namespace fs
{

	bool add_path_protocol(const path& protocol, const path& dir)
	{
		// Protocol matching is case insensitive, convert to lower case
		std::string strNewProtocol = string_utils::toLower(protocol.string());

		auto& protocols = get_path_protocols();
		// Add to the list
		protocols[strNewProtocol] = dir.string();

		// Success!
		return true;
	}

	ProtocolMap& get_path_protocols()
	{
		static ProtocolMap mProtocols;
		return mProtocols;
	}

	ByteArray read_stream(std::istream & stream)
	{
		// Open the stream
		ByteArray read_memory;
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

		auto itProtocol = protocols.find(root);

		if (itProtocol == std::end(protocols))
			return path{};

		const auto resolvedPath = itProtocol->second;
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
		path full_path(
			system_complete(
				path(std::string(argv0)), std::error_code{}));
		return full_path;
	}
}
#if defined(_WIN32)

#  include <Windows.h>
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
}
#elif defined(__APPLE__)

#  include <mach-o/dyld.h>
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
		path full_path(
			system_complete(
				path(std::string(buf)).normalize(), std::error_code{}));
		return full_path;
	}
}
#elif defined(sun) || defined(__sun)

#  include <stdlib.h>
namespace fs
{
	path executable_path(const char *argv0)
	{
		return path(std::string(getexecname()));
	}
}
#elif defined(__FreeBSD__)

#  include <sys/sysctl.h>
namespace fs
{
	path executable_path(const char *argv0)
	{
		int mib[4] = { 0 };
		mib[0] = CTL_KERN;
		mib[1] = KERN_PROC;
		mib[2] = KERN_PROC_PATHNAME;
		mib[3] = -1;
		char buf[1024] = { 0 };
		size_t size = sizeof(buf);
		sysctl(mib, 4, buf, &size, NULL, 0);
		if (size == 0 || size == sizeof(buf))
		{
			return executable_path_fallback(argv0);
		}
		std::string path(buf, size);
		path full_path(
			system_complete(
				path(path).normalize(), std::error_code{}));
		return full_path;
	}
}
#elif defined(__linux__)

#  include <unistd.h>
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
		path full_path(
			system_complete(
				path(path).normalize(), std::error_code{}));
		return full_path;
	}
}
#else
namespace fs
{
	path executable_path(const char *argv0)
	{
		return executable_path_fallback(argv0);
	}
}
#endif

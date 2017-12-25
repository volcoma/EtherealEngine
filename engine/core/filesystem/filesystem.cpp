#include "filesystem.h"
#include "../common/platform_config.h"
#include "../string_utils/string_utils.h"
#include <array>
namespace fs
{

template <typename Container = std::string, typename CharT = char, typename Traits = std::char_traits<char>>
auto read_stream_into_container(std::basic_istream<CharT, Traits>& in,
								typename Container::allocator_type alloc = {})
{
	static_assert(
		// Allow only strings...
		std::is_same<Container,
					 std::basic_string<CharT, Traits, typename Container::allocator_type>>::value ||
			// ... and vectors of the plain, signed, and
			// unsigned flavours of CharT.
			std::is_same<Container, std::vector<CharT, typename Container::allocator_type>>::value ||
			std::is_same<Container, std::vector<std::make_unsigned_t<CharT>,
												typename Container::allocator_type>>::value ||
			std::is_same<Container,
						 std::vector<std::make_signed_t<CharT>, typename Container::allocator_type>>::value,
		"only strings and vectors of ((un)signed) CharT allowed");

	auto const start_pos = in.tellg();
	if(std::streamsize(-1) == start_pos)
		throw std::ios_base::failure{"error"};

	if(!in.ignore(std::numeric_limits<std::streamsize>::max()))
		throw std::ios_base::failure{"error"};

	auto const char_count = in.gcount();

	if(!in.seekg(start_pos))
		throw std::ios_base::failure{"error"};

	auto container = Container(std::move(alloc));
	container.resize(static_cast<std::size_t>(char_count));

	if(0 != container.size())
	{
		if(!in.read(reinterpret_cast<CharT*>(&container[0]), char_count))
			throw std::ios_base::failure{"error"};
	}

	return container;
}

byte_array_t read_stream(std::istream& stream)
{
	return read_stream_into_container<byte_array_t>(stream);
}

bool add_path_protocol(const std::string& protocol, const path& dir)
{
	// Protocol matching is case insensitive, convert to lower case
	auto protocol_lower = string_utils::to_lower(protocol);

	auto& protocols = get_path_protocols();
	// Add to the list
	protocols[protocol_lower] = fs::path(dir).make_preferred().string();

	// Success!
	return true;
}

protocols_t& get_path_protocols()
{
	static protocols_t protocols;
	return protocols;
}

path resolve_protocol(const path& _path)
{
	const auto string_path = _path.generic_string();
	auto pos = string_path.find(':', 0) + 1;
	if(pos == std::string::npos)
		return path{};

	const auto root = string_path.substr(0, pos);

	fs::path relative_path = string_path.substr(pos + 1);
	// Matching path protocol in our list?
	const auto& protocols = get_path_protocols();

	auto it = protocols.find(root);

	if(it == std::end(protocols))
		return path{};

	const auto resolved = it->second;
	auto result = resolved / relative_path.make_preferred();
	return result;
}

bool has_known_protocol(const path& _path)
{
	const auto string_path = _path.generic_string();
	auto pos = string_path.find(':', 0) + 1;
	if(pos == std::string::npos)
		return false;

	const auto root = string_path.substr(0, pos);

	const auto& protocols = get_path_protocols();

	// Matching path protocol in our list?
	return (protocols.find(root) != std::end(protocols));
}

path convert_to_protocol(const path& _path)
{
	const auto string_path = fs::path(_path).make_preferred().string();

	const auto& protocols = get_path_protocols();

	for(const auto& protocol_pair : protocols)
	{
		const auto& protocol = protocol_pair.first;
		const auto& resolved_protocol = protocol_pair.second;

		if(string_utils::begins_with(string_path, resolved_protocol))
		{
			return fs::path(string_utils::replace(string_path, resolved_protocol, protocol)).generic_path();
		}
	}
	return _path;
}
}

#include <stdio.h>
#include <stdlib.h>

namespace fs
{
path executable_path_fallback(const char* argv0)
{
	if(0 == argv0 || 0 == argv0[0])
	{
		return "";
	}
	fs::error_code err;
	path full_path(system_complete(path(std::string(argv0)), err));
	return full_path;
}
}
#if $on($windows)
#include <Windows.h>
namespace fs
{
path executable_path(const char* argv0)
{
	std::array<char, 1024> buf;
	buf.fill(0);
	DWORD ret = GetModuleFileNameA(NULL, buf.data(), DWORD(buf.size()));
	if(ret == 0 || std::size_t(ret) == buf.size())
	{
		return executable_path_fallback(argv0);
	}
	return path(std::string(buf.data()));
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
	path full_path(system_complete(path(std::string(buf.data())).normalize(), err));
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
	path full_path(system_complete(path(p).normalize(), err));
	return full_path;
}
void show_in_graphical_env(const path& _path)
{
	static std::string cmd = "xdg-open";
	static std::string space = " ";
	const std::string cmd_args = "'" + _path.string() + "'";
	const std::string whole_command = cmd + space + cmd_args;
	system(whole_command.c_str());
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

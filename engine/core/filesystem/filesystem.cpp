#include "filesystem.h"
#include <algorithm>

namespace fs
{

namespace detail
{
bool begins_with(const std::string& str, const std::string& value)
{
	// Validate requirements
	if(str.length() < value.length())
		return false;
	if(str.empty() == true || value.empty() == true)
		return false;

	// Do the subsets match?
	auto s1 = str.substr(0, value.length());
	if(s1.compare(value) == 0)
		return true;

	// No match
	return false;
}
static std::string replace_seq(const std::string& str, const std::string& old_sequence,
							   const std::string& new_sequence)
{
	std::string s = str;
	std::string::size_type location = 0;
	std::string::size_type old_length = old_sequence.length();
	std::string::size_type new_length = new_sequence.length();

	// Search for all replace std::string occurances.
	if(s.empty() == false)
	{
		while(std::string::npos != (location = s.find(old_sequence, location)))
		{
			s.replace(location, old_length, new_sequence);
			location += new_length;

			// Break out if we're done
			if(location >= s.length())
				break;

		} // Next

	} // End if not empty

	return s;
}

std::string to_lower(const std::string& str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}

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
}
byte_array_t read_stream(std::istream& stream)
{
	return detail::read_stream_into_container<byte_array_t>(stream);
}

bool add_path_protocol(const std::string& protocol, const path& dir)
{
	// Protocol matching is case insensitive, convert to lower case
	auto protocol_lower = detail::to_lower(protocol);

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

		if(detail::begins_with(string_path, resolved_protocol))
		{
			return replace(string_path, resolved_protocol, protocol).generic_string();
		}
	}
	return _path;
}

path replace(const path& _path, const path& _sequence, const path& _new_sequence)
{
	return path(detail::replace_seq(_path.string(), _sequence.string(), _new_sequence.string()));
}

std::vector<path> split_until(const path& _path, const path& _predicate)
{
	std::vector<path> result;

	auto f = _path;

	while(f.has_parent_path() && f.has_filename() && f != _predicate)
	{
		result.push_back(f);
		f = f.parent_path();
	}

	result.push_back(_predicate);
	std::reverse(std::begin(result), std::end(result));

	return result;
}

path reduce_trailing_extensions(const path& _path)
{
	fs::path reduced = _path;
	for(auto temp = reduced; temp.has_extension(); temp = reduced.stem())
	{
		reduced = temp;
	}

	fs::path result = _path;
	result.remove_filename();
	result /= reduced;
	return result;
}
}

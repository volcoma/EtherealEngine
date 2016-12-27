#pragma once

#include <unordered_map>
#include <istream>
#include <filesystem>

namespace fs
{
	using namespace std::experimental::filesystem;
	using ProtocolMap = std::unordered_map<std::string, std::string>;
	using ByteArray = std::vector<std::uint8_t>;

	//-----------------------------------------------------------------------------
	//  Name : add_path_protocol ()
	/// <summary>
	/// Allows us to map a protocol to a specific directory. A path protocol
	/// gives the caller the ability to prepend an identifier to their file
	/// name i.e. "engine_data://Textures/MyTex.png" and have it return the
	/// relevant mapped path.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool add_path_protocol(const path& protocol, const path& directory);

	//-----------------------------------------------------------------------------
	//  Name : get_path_protocols ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ProtocolMap& get_path_protocols();

	//-----------------------------------------------------------------------------
	//  Name : read_stream ()
	/// <summary>
	/// Load a ByteArray with the contents of the specified file, be that file in a
	/// package or in the main file system.
	/// </summary>
	//-----------------------------------------------------------------------------
	ByteArray read_stream(std::istream& stream);

	//-----------------------------------------------------------------------------
	//  Name : resolve_protocol()
	/// <summary>
	/// Given the specified path/filename, resolve the final full filename.
	/// This will be based on either the currently specified root path,
	/// or one of the 'path protocol' identifiers if it is included in the
	/// filename.
	/// </summary>
	//-----------------------------------------------------------------------------
	path resolve_protocol(const path& _path);

	//-----------------------------------------------------------------------------
	//  Name : executable_path()
	/// <summary>
	/// Retrieve the directory of the currently running application.
	/// </summary>
	//-----------------------------------------------------------------------------
	path executable_path(const char *argv0);
};


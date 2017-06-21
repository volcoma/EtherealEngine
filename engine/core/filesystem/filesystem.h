#pragma once

#include <unordered_map>
#include <istream>
#include "boost/filesystem.hpp"
#include <chrono>
namespace fs
{
	using file_time_type = std::time_t;
	using error_code = boost::system::error_code;
    using namespace boost::filesystem;
	using protocols_t = std::unordered_map<std::string, std::string>;
	using byte_array_t = std::vector<char>;

	//-----------------------------------------------------------------------------
	//  Name : add_path_protocol ()
	/// <summary>
	/// Allows us to map a protocol to a specific directory. A path protocol
	/// gives the caller the ability to prepend an identifier to their file
	/// name i.e. "engine_data:/textures/tex.png" and have it return the
	/// relevant mapped path.
	/// </summary>
	//-----------------------------------------------------------------------------
    bool add_path_protocol(const std::string& protocol, const path& directory);

	//-----------------------------------------------------------------------------
	//  Name : get_path_protocols ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	protocols_t& get_path_protocols();

	//-----------------------------------------------------------------------------
	//  Name : read_stream ()
	/// <summary>
	/// Load a byte_array_t with the contents of the specified file, be that file in a
	/// package or in the main file system.
	/// </summary>
	//-----------------------------------------------------------------------------
	byte_array_t read_stream(std::istream& stream);

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

	//-----------------------------------------------------------------------------
	//  Name : show_in_graphical_env ()
	/// <summary>
	/// Shows a path in the graphical environment e.g Explorer, Finder... etc.
	/// </summary>
	//-----------------------------------------------------------------------------
	void show_in_graphical_env(const path& _path);
};


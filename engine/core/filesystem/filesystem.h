#pragma once

#include "detail/filesystem_includes.h"
#include "detail/filesystem_os.h"

#include <istream>
#include <unordered_map>
#include <vector>

namespace fs
{

using protocols_t = std::unordered_map<std::string, std::string>;
using byte_array_t = std::vector<std::uint8_t>;

//-----------------------------------------------------------------------------
//  Name : add_path_protocol ()
/// <summary>
/// Allows us to map a protocol to a specific directory. A path protocol
/// gives the caller the ability to prepend an identifier to their file
/// name i.e. "engine_data:/textures/tex.png" and have it return the
/// relevant mapped path.
/// </summary>
//-----------------------------------------------------------------------------
auto add_path_protocol(const std::string& protocol, const path& directory) -> bool;

//-----------------------------------------------------------------------------
//  Name : get_path_protocols ()
/// <summary>
/// Returns the registered path protocols.
/// </summary>
//-----------------------------------------------------------------------------
auto get_path_protocols() -> protocols_t&;

//-----------------------------------------------------------------------------
//  Name : resolve_protocol()
/// <summary>
/// Given the specified path/filename, resolve the final full filename.
/// This will be based on either the currently specified root path,
/// or one of the 'path protocol' identifiers if it is included in the
/// filename.
/// </summary>
//-----------------------------------------------------------------------------
auto resolve_protocol(const path& _path) -> path;

//-----------------------------------------------------------------------------
//  Name : convert_to_protocol()
/// <summary>
/// Oposite of the resolve_protocol this function tries to convert to protocol
/// path from an absolute one.
/// </summary>
//-----------------------------------------------------------------------------
auto convert_to_protocol(const path& _path) -> path;

//-----------------------------------------------------------------------------
//  Name : has_known_protocol()
/// <summary>
/// Checks whether the path has a known protocol.
/// </summary>
//-----------------------------------------------------------------------------
auto has_known_protocol(const path& _path) -> bool;

//-----------------------------------------------------------------------------
//  Name : read_stream ()
/// <summary>
/// Load a byte_array_t with the contents of the specified file, be that file in
/// a package or in the main file system.
/// </summary>
//-----------------------------------------------------------------------------
auto read_stream(std::istream& stream) -> byte_array_t;

//-------------------------------------------------------------------------
//  Name : replace ()
/// <summary>
/// Replacing any occurences of the specified path sequence with
/// another.
/// </summary>
//-------------------------------------------------------------------------
auto replace(const path& _path, const path& _sequence, const path& _new_sequence) -> path;

//-------------------------------------------------------------------------
//  Name : split_until ()
/// <summary>
/// another.
/// </summary>
//-------------------------------------------------------------------------
auto split_until(const path& _path, const path& _predicate) -> std::vector<path>;

//-------------------------------------------------------------------------
//  Name : reduce_trailing_extensions ()
/// <summary>
/// another.
/// </summary>
//-------------------------------------------------------------------------
auto reduce_trailing_extensions(const path& _path) -> path;

} // namespace fs

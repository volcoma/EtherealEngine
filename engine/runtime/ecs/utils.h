#pragma once

#include "core/filesystem/filesystem.h"
#include "ecs.h"
#include <fstream>
#include <vector>

namespace ecs
{
namespace utils
{
//-----------------------------------------------------------------------------
//  Name : save_entity ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
void save_entity(const fs::path& dir, const runtime::entity& data);

//-----------------------------------------------------------------------------
//  Name : try_load_entity ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool try_load_entity(const fs::path& fullPath, runtime::entity& outData);

//-----------------------------------------------------------------------------
//  Name : save_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
void save_data(const fs::path& fullPath, const std::vector<runtime::entity>& data);

//-----------------------------------------------------------------------------
//  Name : load_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool load_data(const fs::path& fullPath, std::vector<runtime::entity>& outData);

//-----------------------------------------------------------------------------
//  Name : serialize_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
void serialize_data(std::ostream& stream, const std::vector<runtime::entity>& data);

//-----------------------------------------------------------------------------
//  Name : deserialize_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool deserialize_data(std::istream& stream, std::vector<runtime::entity>& outData);
}
}

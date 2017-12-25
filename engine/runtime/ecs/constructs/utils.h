#pragma once

#include "core/filesystem/filesystem.h"
#include "../ecs.h"
#include <fstream>
#include <vector>

namespace ecs
{
namespace utils
{

runtime::entity clone_entity(const runtime::entity& data);
//-----------------------------------------------------------------------------
//  Name : save_entity ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
void save_entity_to_file(const fs::path& full_path, const runtime::entity& data);

//-----------------------------------------------------------------------------
//  Name : try_load_entity ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool try_load_entity_from_file(const fs::path& full_path, runtime::entity& out_data);

//-----------------------------------------------------------------------------
//  Name : save_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
void save_entities_to_file(const fs::path& full_path, const std::vector<runtime::entity>& data);

//-----------------------------------------------------------------------------
//  Name : load_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool load_entities_from_file(const fs::path& full_path, std::vector<runtime::entity>& out_data);

//-----------------------------------------------------------------------------
//  Name : deserialize_data ()
/// <summary>
///
///
///
/// </summary>
//-----------------------------------------------------------------------------
bool deserialize_data(std::istream& stream, std::vector<runtime::entity>& out_data);
}
}

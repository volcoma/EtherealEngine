#pragma once
#include <string>
#include "asset_handle.h"
#include "../system/fileSystem.h"

class material;
struct asset_writer
{
	//-----------------------------------------------------------------------------
	//  Name : write_material_to_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void write_material_to_file(const fs::path& absoluteKey, const asset_handle<material>& asset);
};
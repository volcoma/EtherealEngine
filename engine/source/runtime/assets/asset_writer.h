#pragma once
#include <string>
#include "asset_handle.h"
#include "../system/fileSystem.h"

class Material;
struct AssetWriter
{
	//-----------------------------------------------------------------------------
	//  Name : write_material_to_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void write_material_to_file(const fs::path& absoluteKey, const AssetHandle<Material>& asset);
};
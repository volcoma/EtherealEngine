#pragma once
#include <string>
#include "asset_handle.h"
#include "../system/fileSystem.h"

class Material;
struct AssetWriter
{
	//-----------------------------------------------------------------------------
	//  Name : saveMaterialToFile ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void saveMaterialToFile(const fs::path& absoluteKey, const AssetHandle<Material>& asset);
};
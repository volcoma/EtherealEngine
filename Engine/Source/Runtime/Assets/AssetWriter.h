#pragma once
#include <string>
#include "AssetHandle.h"

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
	static void saveMaterialToFile(const std::string& absoluteKey, const AssetHandle<Material>& asset);
};
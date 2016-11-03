#pragma once
#include <string>
#include "AssetHandle.h"

class Material;

struct AssetWriter
{
	static void saveMaterialToFile(const std::string& absoluteKey, const AssetHandle<Material>& asset);
};
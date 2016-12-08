#include "AssetWriter.h"
#include "../Rendering/Material.h"

#include "Core/serialization/archives.h"
#include "Meta/Rendering/Material.hpp"

void AssetWriter::saveMaterialToFile(const std::string& absoluteKey, const AssetHandle<Material>& asset)
{
	std::ofstream output(absoluteKey);
	cereal::OArchive_JSON ar(output);
	ar(cereal::make_nvp("material", asset.link->asset));
}
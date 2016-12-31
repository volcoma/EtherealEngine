#include "asset_writer.h"
#include "../rendering/material.h"

#include "core/serialization/archives.h"
#include "meta/rendering/material.hpp"

void AssetWriter::saveMaterialToFile(const fs::path& absoluteKey, const AssetHandle<Material>& asset)
{
	std::ofstream output(absoluteKey);
	cereal::OArchive_JSON ar(output);
	ar(cereal::make_nvp("material", asset.link->asset));
}
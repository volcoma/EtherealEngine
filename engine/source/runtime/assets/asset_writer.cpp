#include "asset_writer.h"
#include "../rendering/material.h"

#include "core/serialization/archives.h"
#include "meta/rendering/material.hpp"

void asset_writer::write_material_to_file(const fs::path& absoluteKey, const asset_handle<material>& asset)
{
	std::ofstream output(absoluteKey);
	cereal::oarchive_json_t ar(output);

	try_save(ar, cereal::make_nvp("material", asset.link->asset));
}
#include "asset_writer.h"
#include "../rendering/material.h"

#include "core/serialization/associative_archive.h"
#include "../meta/rendering/material.hpp"

namespace runtime
{
	template<>
	void asset_writer::save_to_file<material>(const fs::path& key, const asset_handle<material>& asset)
	{
		fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
		std::ofstream output(absolute_key.string());
		cereal::oarchive_associative_t ar(output);

		try_save(ar, cereal::make_nvp("material", asset.link->asset));
	}
}
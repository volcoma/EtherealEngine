#include "asset_writer.h"

#include "../../meta/rendering/material.hpp"

#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"

#include <fstream>
namespace runtime
{
namespace asset_writer
{

template <>
void save_to_file<material>(const fs::path& key, const asset_handle<material>& asset)
{
	fs::path absolute_key = fs::absolute(fs::resolve_protocol(key).string());
	std::ofstream output(absolute_key.string());
	cereal::oarchive_associative_t ar(output);

	try_save(ar, cereal::make_nvp("material", asset.link->asset));
}
}
}

#include "utils.h"
#include "../../meta/ecs/entity.hpp"

#include <core/serialization/associative_archive.h>
#include <core/serialization/binary_archive.h>
#include <core/serialization/serialization.h>

namespace ecs
{
namespace utils
{

template <typename OArchive>
static void serialize_t(std::ostream& stream, const std::vector<runtime::entity>& data)
{
	OArchive ar(stream);

	try_save(ar, cereal::make_nvp("data", data));

	runtime::get_serialization_map().clear();
}

template <typename IArchive>
static bool deserialize_t(std::istream& stream, std::vector<runtime::entity>& out_data)
{
	// get length of file:
	runtime::get_serialization_map().clear();
	stream.seekg(0, stream.end);
	std::streampos length = stream.tellg();
	stream.seekg(0, stream.beg);
	if(length > 0)
	{
		IArchive ar(stream);

		try_load(ar, cereal::make_nvp("data", out_data));

		stream.clear();
		stream.seekg(0);
		runtime::get_serialization_map().clear();
		return true;
	}
	return false;
}

void save_entity_to_file(const fs::path& full_path, const runtime::entity& data)
{
	save_entities_to_file(full_path, {data});
}

bool try_load_entity_from_file(const fs::path& full_path, runtime::entity& out_data)
{
	std::vector<runtime::entity> out_data_vec;
	if(!load_entities_from_file(full_path, out_data_vec))
		return false;

	if(!out_data_vec.empty())
		out_data = out_data_vec[0];

	return true;
}

void save_entities_to_file(const fs::path& full_path, const std::vector<runtime::entity>& data)
{
	std::ofstream os(full_path.string(), std::fstream::binary | std::fstream::trunc);
	serialize_t<cereal::oarchive_associative_t>(os, data);
}

bool load_entities_from_file(const fs::path& full_path, std::vector<runtime::entity>& out_data)
{
	std::ifstream is(full_path.string(), std::fstream::binary);
	return deserialize_t<cereal::iarchive_associative_t>(is, out_data);
}

runtime::entity clone_entity(const runtime::entity& data)
{
	std::stringstream stream;
	serialize_t<cereal::oarchive_binary_t>(stream, {data});

	std::vector<runtime::entity> vec_data;
	deserialize_t<cereal::iarchive_binary_t>(stream, vec_data);

	if(!vec_data.empty())
	{
		return vec_data.front();
	}
	return {};
}

bool deserialize_data(std::istream& stream, std::vector<runtime::entity>& out_data)
{
	return deserialize_t<cereal::iarchive_associative_t>(stream, out_data);
}
}
}

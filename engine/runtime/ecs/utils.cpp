#include "utils.h"
#include "core/serialization/serialization.h"
#include "core/serialization/associative_archive.h"
#include "../Meta/Ecs/Entity.hpp"
#include "../assets/asset_extensions.h"

namespace ecs
{
	namespace utils
	{
		void save_entity(const fs::path& dir, const runtime::entity& data)
		{
			const fs::path fullPath = dir / fs::path(data.to_string() + extensions::prefab);
			save_data(fullPath, { data });
		}

		bool try_load_entity(const fs::path& fullPath, runtime::entity& outData)
		{
			std::vector<runtime::entity> outDataVec;
			if (!load_data(fullPath, outDataVec))
				return false;

			if (!outDataVec.empty())
				outData = outDataVec[0];

			return true;
		}


		void save_data(const fs::path& fullPath, const std::vector<runtime::entity>& data)
		{
			std::ofstream os(fullPath.string(), std::fstream::binary | std::fstream::trunc);
			serialize_data(os, data);
		}

		bool load_data(const fs::path& fullPath, std::vector<runtime::entity>& outData)
		{
			std::ifstream is(fullPath.string(), std::fstream::binary);
			return deserialize_data(is, outData);
		}

		void serialize_data(std::ostream& stream, const std::vector<runtime::entity>& data)
		{
            cereal::oarchive_associative_t ar(stream);

			try_save(ar, cereal::make_nvp("data", data));

			get_serialization_map().clear();
		}

		bool deserialize_data(std::istream& stream, std::vector<runtime::entity>& outData)
		{
			// get length of file:
			get_serialization_map().clear();
			stream.seekg(0, stream.end);
			std::streampos length = stream.tellg();
			stream.seekg(0, stream.beg);
			if (length > 0)
			{
                cereal::iarchive_associative_t ar(stream);

				try_load(ar, cereal::make_nvp("data", outData));

				stream.clear();
				stream.seekg(0);
				get_serialization_map().clear();
				return true;
			}
			return false;
		}
	}
}

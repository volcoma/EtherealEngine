#include "Utils.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/archives.h"
#include "../Meta/Ecs/Entity.hpp"

namespace ecs
{
	namespace utils
	{
		void saveEntity(const std::string& name, const Entity& data)
		{
			saveData(name, { data });
		}

		bool tryLoadEntity(const std::string& name, Entity& outData)
		{
			std::vector<Entity> outDataVec;
			if (!loadData(name, outDataVec))
				return false;

			if (!outDataVec.empty())
				outData = outDataVec[0];

			return true;
		}


		void saveData(const std::string& fullPath, const std::vector<Entity>& data)
		{
			std::ofstream os(fullPath, std::fstream::binary | std::fstream::trunc);
			serializeData(os, data);
		}

		bool loadData(const std::string& fullPath, std::vector<Entity>& outData)
		{
			std::ifstream is(fullPath, std::fstream::binary);
			return deserializeData(is, outData);
		}

		void serializeData(std::ostream& stream, const std::vector<Entity>& data)
		{
			cereal::OArchive_Binary ar(stream);

			ar(
				cereal::make_nvp("data", data)
			);
			getSerializationMap().clear();
		}

		bool deserializeData(std::istream& stream, std::vector<Entity>& outData)
		{
			// get length of file:
			getSerializationMap().clear();
			stream.seekg(0, stream.end);
			std::streampos length = stream.tellg();
			stream.seekg(0, stream.beg);
			if (length > 0)
			{
				cereal::IArchive_Binary ar(stream);

				ar(
					cereal::make_nvp("data", outData)
				);

				stream.clear();
				stream.seekg(0);
				getSerializationMap().clear();
				return true;
			}
			return false;
		}
	}
}

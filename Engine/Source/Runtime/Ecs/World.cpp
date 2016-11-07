#include "World.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/archives.h"
#include "../Meta/Ecs/Entity.hpp"

World::World() : entities(events), systems(entities, events)
{

}

void World::reset()
{
	entities.reset();
}

void World::saveEntity(const std::string& name, const ecs::Entity& data)
{
	saveData(name, { data });
}

bool World::tryLoadEntity(const std::string& name, ecs::Entity& outData)
{
	std::vector<Entity> outDataVec;
	if (!loadData(name, outDataVec))
		return false;

	if (!outDataVec.empty())
		outData = outDataVec[0];

	return true;
}


void World::saveData(const std::string& fullPath, const std::vector<ecs::Entity>& data)
{
	std::ofstream os(fullPath, std::fstream::binary | std::fstream::trunc);
	serializeData(os, data);
}

bool World::loadData(const std::string& fullPath, std::vector<ecs::Entity>& outData)
{
	std::ifstream is(fullPath, std::fstream::binary);
	return deserializeData(is, outData);
}

void World::serializeData(std::ostream& stream, const std::vector<ecs::Entity>& data)
{
	cereal::OArchive_Binary ar(stream);

	ar(
		cereal::make_nvp("data", data)
	);
	deserialized.clear();
}

bool World::deserializeData(std::istream& stream, std::vector<ecs::Entity>& outData)
{
	// get length of file:
	deserialized.clear();
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
		deserialized.clear();
		return true;
	}
	return false;
}
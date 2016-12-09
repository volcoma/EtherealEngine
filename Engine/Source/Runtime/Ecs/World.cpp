#include "World.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/archives.h"
#include "../Meta/Ecs/Entity.hpp"
#include "Utils.h"

World::World() : entities(events), systems(entities, events)
{

}

void World::reset()
{
	entities.reset();
}

Entity Prefab::instantiate()
{
	std::vector<Entity> outDataVec;
	if (!ecs::utils::deserializeData(data, outDataVec))
		return Entity();

	if (outDataVec.empty())
		return Entity();
	else
		return outDataVec[0];

}

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
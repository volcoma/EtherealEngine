#pragma once

#include "entityx/quick.h"
#include <vector>
#include <fstream>
using namespace entityx;

struct World
{
	World();
	EventManager events;
	EntityManager entities;
	SystemManager systems;

	//TODO move this to a more appropriate place
	std::map<uint32_t, Entity> deserialized;

	void reset();
	void saveEntity(const std::string& name, const Entity& data);
	bool tryLoadEntity(const std::string& name, Entity& outData);
	void saveData(const std::string& fullPath, const std::vector<Entity>& data);
	bool loadData(const std::string& fullPath, std::vector<Entity>& outData);
	void serializeData(std::ostream& stream, const std::vector<Entity>& data);
	bool deserializeData(std::istream& stream, std::vector<ecs::Entity>& outData);
};

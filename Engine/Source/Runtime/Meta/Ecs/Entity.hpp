#pragma once
#include "../../Ecs/entityx/Entity.h"
#include "Core/reflection/reflection.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/vector.hpp"
#include "../../Application/Application.h"
#include "../../System/Singleton.h"
#include "../../Ecs/World.h"

namespace entityx
{


SAVE(Entity)
{
	auto& app = Singleton<Application>::getInstance();
	auto& world = app.getWorld();
	auto id = obj.id().index();
	ar(
		cereal::make_nvp("entity_id", id)
	);
	auto it = world.deserialized.find(id);
	if (it == world.deserialized.end())
	{
		world.deserialized[id] = obj;
		ar(
			cereal::make_nvp("name", obj.getName()),
			cereal::make_nvp("components", obj.all_components())
		);
	}

}

LOAD(Entity)
{
	uint32_t id;
	std::string name;
	std::vector<ComponentHandle<Component>> components;

	ar(
		cereal::make_nvp("entity_id", id)
	);

	
	auto& app = Singleton<Application>::getInstance();
	auto& world = app.getWorld();
	auto it = world.deserialized.find(id);
	if (it != world.deserialized.end())
	{
		obj = it->second;
	}
	else
	{
		obj = world.entities.create();
		world.deserialized[id] = obj;

		ar(
			cereal::make_nvp("name", name),
			cereal::make_nvp("components", components)
		);
		obj.setName(name);
		for (auto component : components)
		{
			auto component_shared = component.lock();
			obj.assign(component_shared);
			static std::string context = "deserialized";
			component_shared->onModified(context);
		}
	}

}

}
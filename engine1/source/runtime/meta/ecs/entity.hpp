#pragma once
#include "core/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"

inline std::map<uint32_t, core::Entity>& getSerializationMap()
{
	/// Keep count of serialized entities
	static std::map<uint32_t, core::Entity> serializationMap;
	return serializationMap;
}


namespace core
{

SAVE(Entity)
{
	auto id = obj.id().index();
	ar(
		cereal::make_nvp("entity_id", id)
	);
	auto& serializationMap = getSerializationMap();
	auto it = serializationMap.find(id);
	if (it == serializationMap.end())
	{
		serializationMap[id] = obj;
		ar(
			cereal::make_nvp("name", obj.get_name()),
			cereal::make_nvp("components", obj.all_components())
		);
	}

}

LOAD(Entity)
{
	std::uint32_t id;
	std::string name;
	std::vector<CHandle<Component>> components;

	ar(
		cereal::make_nvp("entity_id", id)
	);

	auto& serializationMap = getSerializationMap();
	auto it = serializationMap.find(id);
	if (it != serializationMap.end())
	{
		obj = it->second;
	}
	else
	{
		auto ecs = get_subsystem<EntityComponentSystem>();
		obj = ecs->create();
		serializationMap[id] = obj;

		ar(
			cereal::make_nvp("name", name),
			cereal::make_nvp("components", components)
		);
		obj.set_name(name);
		for (auto component : components)
		{
			auto component_shared = component.lock();
			obj.assign(component_shared);
			static const std::string context = "deserialized";
			component_shared->touch(context);
		}
	}

}

}

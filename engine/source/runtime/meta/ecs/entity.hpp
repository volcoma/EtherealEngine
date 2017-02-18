#pragma once
#include "../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/logging/logging.h"

inline std::map<uint32_t, runtime::Entity>& getSerializationMap()
{
	/// Keep count of serialized entities
	static std::map<uint32_t, runtime::Entity> serializationMap;
	return serializationMap;
}


namespace runtime
{

SAVE(Entity)
{

	auto id = obj.id().index();
	try_save(ar, cereal::make_nvp("entity_id", id));
	
	auto& serializationMap = getSerializationMap();
	auto it = serializationMap.find(id);
	if (it == serializationMap.end())
	{
		serializationMap[id] = obj;
		
		try_save(ar, cereal::make_nvp("name", obj.get_name()));
		try_save(ar, cereal::make_nvp("components", obj.all_components()));
	}
}

LOAD(Entity)
{
	std::uint32_t id;
	std::string name;
	std::vector<CHandle<Component>> components;

	
	try_load(ar, cereal::make_nvp("entity_id", id));
	

	auto& serializationMap = getSerializationMap();
	auto it = serializationMap.find(id);
	if (it != serializationMap.end())
	{
		obj = it->second;
	}
	else
	{
		auto ecs = core::get_subsystem<EntityComponentSystem>();
		obj = ecs->create();
		serializationMap[id] = obj;

		try_load(ar, cereal::make_nvp("name", name));
		try_load(ar, cereal::make_nvp("components", components));
		
		obj.set_name(name);
		for (auto component : components)
		{
			auto component_shared = component.lock();
			obj.assign(component_shared);
			component_shared->touch();
		}
	}

}

}

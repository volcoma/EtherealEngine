#pragma once
#include "../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/vector.hpp"
#include "core/logging/logging.h"

inline std::map<uint32_t, runtime::entity>& get_serialization_map()
{
	/// Keep count of serialized entities
	static std::map<uint32_t, runtime::entity> serializationMap;
	return serializationMap;
}


namespace runtime
{

SAVE(entity)
{

	auto id = obj.id().index();
	try_save(ar, cereal::make_nvp("entity_id", id));
	
	auto& serializationMap = get_serialization_map();
	auto it = serializationMap.find(id);
	if (it == serializationMap.end())
	{
		serializationMap[id] = obj;
		
		try_save(ar, cereal::make_nvp("name", obj.get_name()));
		try_save(ar, cereal::make_nvp("components", obj.all_components()));
	}
}

LOAD(entity)
{
	std::uint32_t id;
	std::string name;
	std::vector<chandle<component>> components;

	
	try_load(ar, cereal::make_nvp("entity_id", id));
	

	auto& serializationMap = get_serialization_map();
	auto it = serializationMap.find(id);
	if (it != serializationMap.end())
	{
		obj = it->second;
	}
	else
	{
		auto& ecs = core::get_subsystem<entity_component_system>();
		obj = ecs.create();
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

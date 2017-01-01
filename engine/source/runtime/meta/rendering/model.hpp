#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "../../rendering/model.h"
#include "../assets/asset_handle.hpp"
REFLECT(Model)
{
	rttr::registration::class_<Model>("Model")
		.property("Levels of Detail",
			&Model::get_lods,
			&Model::set_lods)
		.property("Materials",
			&Model::get_materials,
			&Model::set_materials)
		.property("Transition Time",
			&Model::get_lod_transition_time,
			&Model::set_lod_transition_time)
		.property("Max Distance",
			&Model::get_lod_max_distance,
			&Model::set_lod_max_distance)
		.property("Mset_lod_in Distance",
			&Model::get_lod_min_distance,
			&Model::set_lod_min_distance)
		;
}

SAVE(Model)
{
	ar(
		cereal::make_nvp("lods", obj._mesh_lods),
		cereal::make_nvp("materials", obj._materials),
		cereal::make_nvp("transition_time", obj._transition_time),
		cereal::make_nvp("max_distance", obj._max_distance),
		cereal::make_nvp("min_distance", obj._min_distance)
	);

}

LOAD(Model)
{
	ar(
		cereal::make_nvp("lods", obj._mesh_lods),
		cereal::make_nvp("materials", obj._materials),
		cereal::make_nvp("transition_time", obj._transition_time),
		cereal::make_nvp("max_distance", obj._max_distance),
		cereal::make_nvp("min_distance", obj._min_distance)
	);
}
#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
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
		.property("Min Distance",
			&Model::get_lod_min_distance,
			&Model::set_lod_min_distance)
		;
}

SAVE(Model)
{
	try_save(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_save(ar, cereal::make_nvp("materials", obj._materials));
	try_save(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_save(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_save(ar, cereal::make_nvp("min_distance", obj._min_distance));
}

LOAD(Model)
{

	try_load(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_load(ar, cereal::make_nvp("materials", obj._materials));
	try_load(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_load(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_load(ar, cereal::make_nvp("min_distance", obj._min_distance));
}
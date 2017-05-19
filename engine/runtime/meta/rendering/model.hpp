#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/model.h"
#include "../assets/asset_handle.hpp"

REFLECT(model)
{
	rttr::registration::class_<model>("model")
		.property("Levels of Detail",
			&model::get_lods,
			&model::set_lods)
		.property("Materials",
			&model::get_materials,
			&model::set_materials)
		.property("Transition Time",
			&model::get_lod_transition_time,
			&model::set_lod_transition_time)
		.property("Max Distance",
			&model::get_lod_max_distance,
			&model::set_lod_max_distance)
		.property("Min Distance",
			&model::get_lod_min_distance,
			&model::set_lod_min_distance)
		;
}

SAVE(model)
{
	try_save(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_save(ar, cereal::make_nvp("materials", obj._materials));
	try_save(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_save(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_save(ar, cereal::make_nvp("min_distance", obj._min_distance));
}

LOAD(model)
{

	try_load(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_load(ar, cereal::make_nvp("materials", obj._materials));
	try_load(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_load(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_load(ar, cereal::make_nvp("min_distance", obj._min_distance));
}
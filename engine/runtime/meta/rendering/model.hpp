#pragma once

#include "core/serialization/serialization.h"
#include "core/serialization/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/model.h"
#include "../assets/asset_handle.hpp"

REFLECT(model)
{
	rttr::registration::class_<model>("model")
		.property("lods",
			&model::get_lods,
			&model::set_lods)
		(
			rttr::metadata("pretty_name", "Levels of Detail"),
			rttr::metadata("Tooltip", "Levels of Detail.")
		)
		.property("materials",
			&model::get_materials,
			&model::set_materials)
		(
			rttr::metadata("pretty_name", "Materials"),
			rttr::metadata("Tooltip", "Materials for this model.")
		)
		.property("lod_transition_time",
			&model::get_lod_transition_time,
			&model::set_lod_transition_time)
		(
			rttr::metadata("pretty_name", "Transition Time"),
			rttr::metadata("Tooltip", "Transition time between two levels of detail.")
		)
		.property("lod_max_distance",
			&model::get_lod_max_distance,
			&model::set_lod_max_distance)
		(
			rttr::metadata("pretty_name", "Max Distance"),
			rttr::metadata("Tooltip", "Further from this distance will use the lowest level of detail.")
		)
		.property("lod_min_distance",
			&model::get_lod_min_distance,
			&model::set_lod_min_distance)
		(
			rttr::metadata("pretty_name", "Min Distance"),
			rttr::metadata("Tooltip", "Nearer from this distance will use the highest level of detail.")
		)
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
#pragma once

#include "../../rendering/model.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(model)
{
	rttr::registration::class_<model>("model")
		.property("lods", &model::get_lods, &model::set_lods)(
			rttr::metadata("pretty_name", "Levels of Detail"), rttr::metadata("Tooltip", "Levels of Detail."))
		.property("materials", &model::get_materials,
				  &model::set_materials)(rttr::metadata("pretty_name", "Materials"),
										 rttr::metadata("Tooltip", "Materials for this model."))
		.property("lod_transition_time", &model::get_lod_transition_time, &model::set_lod_transition_time)(
			rttr::metadata("pretty_name", "Transition Time"),
			rttr::metadata("Tooltip", "Transition time between two levels of detail."))
		.property("lod_max_distance", &model::get_lod_max_distance, &model::set_lod_max_distance)(
			rttr::metadata("pretty_name", "Max Distance"),
			rttr::metadata("Tooltip", "Further from this distance will use the "
									  "lowest level of detail."))
		.property("lod_min_distance", &model::get_lod_min_distance, &model::set_lod_min_distance)(
			rttr::metadata("pretty_name", "Min Distance"),
			rttr::metadata("Tooltip", "Nearer from this distance will use the "
									  "highest level of detail."));
}

SAVE_EXTERN(model);
LOAD_EXTERN(model);

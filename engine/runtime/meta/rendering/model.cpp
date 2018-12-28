#include "model.hpp"
#include "material.hpp"
#include "mesh.hpp"

#include "../assets/asset_handle.hpp"
#include "../core/common/basetypes.hpp"

#include <core/serialization/associative_archive.h>
#include <core/serialization/binary_archive.h>
#include <core/serialization/types/vector.hpp>


REFLECT(model)
{
	rttr::registration::class_<model>("model")
		.property("lods", &model::get_lods, &model::set_lods)(rttr::metadata("pretty_name", "LOD"),
															  rttr::metadata("tooltip", "Levels of Detail."))
		.property("lod_limits", &model::get_lod_limits, &model::set_lod_limits)(
			rttr::metadata("pretty_name", "LOD Ranges"),
			rttr::metadata("tooltip", "LOD ranges in % of screen."), rttr::metadata("format", "%.0f%%"),
			rttr::metadata("min", 0), rttr::metadata("max", 100))
		.property("materials", &model::get_materials,
				  &model::set_materials)(rttr::metadata("pretty_name", "Materials"),
										 rttr::metadata("tooltip", "Materials for this model."))
		.property("lod_transition_time", &model::get_lod_transition_time, &model::set_lod_transition_time)(
			rttr::metadata("pretty_name", "Transition Time"),
			rttr::metadata("tooltip", "Transition time between two levels of detail."))

		;
}

SAVE(model)
{
	try_save(ar, cereal::make_nvp("lods", obj.mesh_lods_));
	try_save(ar, cereal::make_nvp("materials", obj.materials_));
	try_save(ar, cereal::make_nvp("transition_time", obj.transition_time_));
	try_save(ar, cereal::make_nvp("lod_limits", obj.lod_limits_));
}
SAVE_INSTANTIATE(model, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(model, cereal::oarchive_binary_t);

LOAD(model)
{

	try_load(ar, cereal::make_nvp("lods", obj.mesh_lods_));
	try_load(ar, cereal::make_nvp("materials", obj.materials_));
	try_load(ar, cereal::make_nvp("transition_time", obj.transition_time_));
	try_load(ar, cereal::make_nvp("lod_limits", obj.lod_limits_));
}
LOAD_INSTANTIATE(model, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(model, cereal::iarchive_binary_t);

#include "model.hpp"
#include "../assets/asset_handle.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/types/vector.hpp"
#include "material.hpp"
#include "mesh.hpp"

SAVE(model)
{
	try_save(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_save(ar, cereal::make_nvp("materials", obj._materials));
	try_save(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_save(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_save(ar, cereal::make_nvp("min_distance", obj._min_distance));
}
SAVE_INSTANTIATE(model, cereal::oarchive_associative_t);

LOAD(model)
{

	try_load(ar, cereal::make_nvp("lods", obj._mesh_lods));
	try_load(ar, cereal::make_nvp("materials", obj._materials));
	try_load(ar, cereal::make_nvp("transition_time", obj._transition_time));
	try_load(ar, cereal::make_nvp("max_distance", obj._max_distance));
	try_load(ar, cereal::make_nvp("min_distance", obj._min_distance));
}
LOAD_INSTANTIATE(model, cereal::iarchive_associative_t);

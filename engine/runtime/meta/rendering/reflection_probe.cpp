#include "reflection_probe.hpp"
#include "core/meta/math/vector.hpp"
#include "core/serialization/associative_archive.h"

SAVE(reflection_probe)
{
	try_save(ar, cereal::make_nvp("type", obj.type));
	try_save(ar, cereal::make_nvp("method", obj.method));
	try_save(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_save(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_save(ar, cereal::make_nvp("range", obj.sphere_data.range));
}
SAVE_INSTANTIATE(reflection_probe, cereal::oarchive_associative_t);

LOAD(reflection_probe)
{
	try_load(ar, cereal::make_nvp("type", obj.type));
	try_load(ar, cereal::make_nvp("method", obj.method));
	try_load(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_load(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_load(ar, cereal::make_nvp("range", obj.sphere_data.range));
}
LOAD_INSTANTIATE(reflection_probe, cereal::iarchive_associative_t);

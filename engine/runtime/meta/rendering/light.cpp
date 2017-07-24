#include "light.hpp"
#include "core/meta/math/vector.hpp"
#include "core/serialization/associative_archive.h"

SAVE(light)
{
	try_save(ar, cereal::make_nvp("type", obj.type));
	try_save(ar, cereal::make_nvp("depth", obj.depth));
	try_save(ar, cereal::make_nvp("shadow", obj.shadow));
	try_save(ar, cereal::make_nvp("spot_range", obj.spot_data.range));
	try_save(ar, cereal::make_nvp("spot_inner_angle", obj.spot_data.inner_angle));
	try_save(ar, cereal::make_nvp("spot_outer_angle", obj.spot_data.outer_angle));
	try_save(ar, cereal::make_nvp("point_range", obj.point_data.range));
	try_save(ar, cereal::make_nvp("point_exponent_falloff", obj.point_data.exponent_falloff));
	try_save(ar, cereal::make_nvp("point_fov_x_adjust", obj.point_data.fov_x_adjust));
	try_save(ar, cereal::make_nvp("point_fov_y_adjust", obj.point_data.fov_y_adjust));
	try_save(ar, cereal::make_nvp("point_stencil_pack", obj.point_data.stencil_pack));
	try_save(ar, cereal::make_nvp("dir_num_splits", obj.directional_data.num_splits));
	try_save(ar, cereal::make_nvp("dir_split_distribution", obj.directional_data.split_distribution));
	try_save(ar, cereal::make_nvp("dir_stabilize", obj.directional_data.stabilize));
	try_save(ar, cereal::make_nvp("intensity", obj.intensity));
	try_save(ar, cereal::make_nvp("color", obj.color));
}
SAVE_INSTANTIATE(light, cereal::oarchive_associative_t);

LOAD(light)
{
	try_load(ar, cereal::make_nvp("type", obj.type));
	try_load(ar, cereal::make_nvp("depth", obj.depth));
	try_load(ar, cereal::make_nvp("shadow", obj.shadow));
	try_load(ar, cereal::make_nvp("spot_range", obj.spot_data.range));
	try_load(ar, cereal::make_nvp("spot_inner_angle", obj.spot_data.inner_angle));
	try_load(ar, cereal::make_nvp("spot_outer_angle", obj.spot_data.outer_angle));
	try_load(ar, cereal::make_nvp("point_range", obj.point_data.range));
	try_load(ar, cereal::make_nvp("point_exponent_falloff", obj.point_data.exponent_falloff));
	try_load(ar, cereal::make_nvp("point_fov_x_adjust", obj.point_data.fov_x_adjust));
	try_load(ar, cereal::make_nvp("point_fov_y_adjust", obj.point_data.fov_y_adjust));
	try_load(ar, cereal::make_nvp("point_stencil_pack", obj.point_data.stencil_pack));
	try_load(ar, cereal::make_nvp("dir_num_splits", obj.directional_data.num_splits));
	try_load(ar, cereal::make_nvp("dir_split_distribution", obj.directional_data.split_distribution));
	try_load(ar, cereal::make_nvp("dir_stabilize", obj.directional_data.stabilize));
	try_load(ar, cereal::make_nvp("intensity", obj.intensity));
	try_load(ar, cereal::make_nvp("color", obj.color));
}
LOAD_INSTANTIATE(light, cereal::iarchive_associative_t);

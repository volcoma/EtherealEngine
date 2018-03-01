#include "animation.hpp"
#include "../core/math/quaternion.hpp"
#include "../core/math/transform.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"

namespace runtime
{
REFLECT(node_animation)
{
	rttr::registration::class_<node_animation>("node_animation")
		.property_readonly("node_name", &node_animation::node_name)(rttr::metadata("pretty_name", "Name"));
}

REFLECT(animation)
{
	rttr::registration::class_<animation>("animation")
		.property_readonly("name", &animation::name)(rttr::metadata("pretty_name", "Name"))
		.property_readonly("duration", &animation::duration)(rttr::metadata("pretty_name", "Duration"))
		.property_readonly("channels", &animation::channels)(rttr::metadata("pretty_name", "Channels"));
}

SAVE(node_animation)
{
	try_save(ar, cereal::make_nvp("node_name", obj.node_name));
	try_save(ar, cereal::make_nvp("position_keys", obj.position_keys));
	try_save(ar, cereal::make_nvp("rotation_keys", obj.rotation_keys));
	try_save(ar, cereal::make_nvp("scaling_keys", obj.scaling_keys));
}
SAVE_INSTANTIATE(node_animation, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(node_animation, cereal::oarchive_binary_t);

LOAD(node_animation)
{
	try_load(ar, cereal::make_nvp("node_name", obj.node_name));
	try_load(ar, cereal::make_nvp("position_keys", obj.position_keys));
	try_load(ar, cereal::make_nvp("rotation_keys", obj.rotation_keys));
	try_load(ar, cereal::make_nvp("scaling_keys", obj.scaling_keys));
}
LOAD_INSTANTIATE(node_animation, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(node_animation, cereal::iarchive_binary_t);

SAVE(animation)
{
	try_save(ar, cereal::make_nvp("name", obj.name));
	try_save(ar, cereal::make_nvp("duration", obj.duration));
	try_save(ar, cereal::make_nvp("ticks_per_second", obj.channels));
}
SAVE_INSTANTIATE(animation, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(animation, cereal::oarchive_binary_t);

LOAD(animation)
{
	try_load(ar, cereal::make_nvp("name", obj.name));
	try_load(ar, cereal::make_nvp("duration", obj.duration));
	try_load(ar, cereal::make_nvp("ticks_per_second", obj.channels));
}
LOAD_INSTANTIATE(animation, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(animation, cereal::iarchive_binary_t);
}

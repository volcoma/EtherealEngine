#include "material.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"

REFLECT(material)
{
	rttr::registration::enumeration<cull_type>("cull_type")(
		rttr::value("none", cull_type::none), rttr::value("clockwise", cull_type::clockwise),
		rttr::value("counter_clockwise", cull_type::counter_clockwise));

	rttr::registration::class_<material>("material")
		.property("cull_type", &material::get_cull_type,
				  &material::set_cull_type)(rttr::metadata("pretty_name", "Cull Type"));
}

SAVE(material)
{
	try_save(ar, cereal::make_nvp("cull_type", obj.cull_type_));
}
SAVE_INSTANTIATE(material, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(material, cereal::oarchive_binary_t);

LOAD(material)
{
	try_load(ar, cereal::make_nvp("cull_type", obj.cull_type_));
}
LOAD_INSTANTIATE(material, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(material, cereal::iarchive_binary_t);

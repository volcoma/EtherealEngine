#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/reflection_probe.h"

REFLECT(reflection_probe)
{
	rttr::registration::enumeration<probe_type>("probe_type")
		(
			rttr::value("Box", probe_type::box),
			rttr::value("Sphere", probe_type::sphere)
		);
	rttr::registration::enumeration<reflect_method>("ReflectMethod")
		(
			rttr::value("Environment", reflect_method::environment),
			rttr::value("Static Only", reflect_method::static_only)
		);
	rttr::registration::class_<reflection_probe::box>("box")
		.property("Extents", &reflection_probe::box::extents)
		.property("Transition Distance", &reflection_probe::box::transition_distance)
		;
	rttr::registration::class_<reflection_probe::sphere>("sphere")
		.property("Range", &reflection_probe::sphere::range)
		;
	rttr::registration::class_<reflection_probe>("reflection_probe")
		.property("Type", &reflection_probe::probe_type)
		.property("Method", &reflection_probe::method)
		;
}

SAVE(reflection_probe)
{
	try_save(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_save(ar, cereal::make_nvp("method", obj.method));
	try_save(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_save(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_save(ar, cereal::make_nvp("range", obj.sphere_data.range));
}

LOAD(reflection_probe)
{
	try_load(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_load(ar, cereal::make_nvp("method", obj.method));
	try_load(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_load(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_load(ar, cereal::make_nvp("range", obj.sphere_data.range));
}
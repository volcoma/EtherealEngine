#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/reflection_probe.h"

REFLECT(ReflectionProbe)
{
	rttr::registration::enumeration<ProbeType>("ProbeType")
		(
			rttr::value("Box", ProbeType::Box),
			rttr::value("Sphere", ProbeType::Sphere)
		);
	rttr::registration::class_<ReflectionProbe::Box>("Box")
		.property("Extents", &ReflectionProbe::Box::extents)
		.property("Transition Distance", &ReflectionProbe::Box::transition_distance)
		;
	rttr::registration::class_<ReflectionProbe::Sphere>("Sphere")
		.property("Range", &ReflectionProbe::Sphere::range)
		;
	rttr::registration::class_<ReflectionProbe>("ReflectionProbe")
		.property("Type", &ReflectionProbe::probe_type)
		;
}

SAVE(ReflectionProbe)
{
	try_save(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_save(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_save(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_save(ar, cereal::make_nvp("range", obj.sphere_data.range));
}

LOAD(ReflectionProbe)
{
	try_load(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_load(ar, cereal::make_nvp("extents", obj.box_data.extents));
	try_load(ar, cereal::make_nvp("transition_distance", obj.box_data.transition_distance));
	try_load(ar, cereal::make_nvp("range", obj.sphere_data.range));
}
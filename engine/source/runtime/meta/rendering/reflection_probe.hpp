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
	rttr::registration::class_<ReflectionProbe>("ReflectionProbe")
		.property("Type", &ReflectionProbe::probe_type)
		(
			rttr::metadata("Tooltip", "Only .x used when sphere type is used.")
		)
		.property("Extents", &ReflectionProbe::extents)
		.property("Transiition Distance", &ReflectionProbe::transition_distance)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		;
}

SAVE(ReflectionProbe)
{
	try_save(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_save(ar, cereal::make_nvp("extents", obj.extents));
	try_save(ar, cereal::make_nvp("transition_distance", obj.transition_distance));
}

LOAD(ReflectionProbe)
{
	try_load(ar, cereal::make_nvp("probe_type", obj.probe_type));
	try_load(ar, cereal::make_nvp("extents", obj.extents));
	try_load(ar, cereal::make_nvp("transition_distance", obj.transition_distance));
}
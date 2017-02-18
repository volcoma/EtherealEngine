#pragma once
#include <cstdint>
#include "core/reflection/rttr/rttr_enable.h"
#include "core/serialization/serialization.h"
#include "core/math/math_includes.h"

enum class ProbeType : std::uint8_t
{
	Box = 0,
	Sphere = 1
};

enum class ReflectMethod : std::uint8_t
{
	Environment = 0,
	Static = 1,
};


struct ReflectionProbe
{
	REFLECTABLE(ReflectionProbe)
	SERIALIZABLE(ReflectionProbe)

	struct Box
	{
		math::vec3 extents = { 5.0, 5.0f, 5.0f };
		float transition_distance = 1.0f;
	};

	struct Sphere
	{
		float range = 5.0f;
	};

	bool operator==(const ReflectionProbe& pr) const
	{
		return probe_type == pr.probe_type &&
			method == pr.method &&
			box_data.extents == pr.box_data.extents &&
			box_data.transition_distance == pr.box_data.transition_distance &&
			sphere_data.range == pr.sphere_data.range;
	}

	bool operator!=(const ReflectionProbe& pr) const
	{
		return !operator==(pr);
	}

	ProbeType probe_type = ProbeType::Box;
	ReflectMethod method = ReflectMethod::Environment;
	Box box_data;
	Sphere sphere_data;
};

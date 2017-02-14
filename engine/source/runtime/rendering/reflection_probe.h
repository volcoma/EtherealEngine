#pragma once
#include <cstdint>
#include "core/reflection/rttr/rttr_enable.h"
#include "core/serialization/serialization.h"
#include "core/math/math_includes.h"

enum class ProbeType : std::uint8_t
{
	Box = 0,
	Sphere = 1,

	Count
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

	ProbeType probe_type = ProbeType::Box;
	Box box_data;
	Sphere sphere_data;
};

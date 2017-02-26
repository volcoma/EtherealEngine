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

	/// The probe type
	ProbeType probe_type = ProbeType::Box;
	/// Reflection Method
	ReflectMethod method = ReflectMethod::Environment;
	/// Data describing box projection
	Box box_data;
	/// Data describing sphere projection
	Sphere sphere_data;
};

inline bool operator==(const ReflectionProbe& pr1, const ReflectionProbe& pr2)
{
	return pr1.probe_type == pr2.probe_type &&
		pr1.method == pr2.method &&
		pr1.box_data.extents == pr2.box_data.extents &&
		pr1.box_data.transition_distance == pr2.box_data.transition_distance &&
		pr1.sphere_data.range == pr2.sphere_data.range;
}

inline bool operator!=(const ReflectionProbe& pr1, const ReflectionProbe& pr2)
{
	return !(pr1==pr2);
}

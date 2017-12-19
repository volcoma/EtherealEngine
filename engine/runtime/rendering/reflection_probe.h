#pragma once
#include "core/math/math_includes.h"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"
#include <cstdint>

enum class probe_type : std::uint8_t
{
	box = 0,
	sphere = 1
};

enum class reflect_method : std::uint8_t
{
	environment = 0,
	static_only = 1,
};

struct reflection_probe
{
	REFLECTABLE(reflection_probe)
	SERIALIZABLE(reflection_probe)

	struct box
	{
		math::vec3 extents = {5.0, 5.0f, 5.0f};
		float transition_distance = 1.0f;
	};

	struct sphere
	{
		float range = 5.0f;
	};

	/// The probe type
	probe_type type = probe_type::box;
	/// Reflection Method
	reflect_method method = reflect_method::environment;
	/// Data describing box projection
	box box_data;
	/// Data describing sphere projection
	sphere sphere_data;
};

inline bool operator==(const reflection_probe& pr1, const reflection_probe& pr2)
{
	return pr1.type == pr2.type && pr1.method == pr2.method && pr1.box_data.extents == pr2.box_data.extents &&
		   math::epsilonEqual(pr1.box_data.transition_distance, pr2.box_data.transition_distance, math::epsilon<float>()) &&
		   math::epsilonEqual(pr1.sphere_data.range, pr2.sphere_data.range, math::epsilon<float>());
}

inline bool operator!=(const reflection_probe& pr1, const reflection_probe& pr2)
{
	return !(pr1 == pr2);
}

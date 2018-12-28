#pragma once

#include <core/math/math_includes.h>
#include <core/reflection/registration.h>
#include <core/serialization/serialization.h>

#include <cstdint>

enum class light_type : std::uint8_t
{
	spot = 0,
	point = 1,
	directional = 2,

	count
};

enum class depth_type : std::uint8_t
{
	invz = 0,
	linear = 1,

	count
};

enum class pack_depth : std::uint8_t
{
	rgba = 0,
	vsm = 1,

	count
};

enum class shadow_type : std::uint8_t
{
	hard = 0,
	pcf = 1,
	vsm = 2,
	esm = 3,

	count
};

enum class sm_type : std::uint8_t
{
	single = 0,
	omni = 1,
	cascade = 2,

	count
};

struct light
{
	REFLECTABLE(light)
	SERIALIZABLE(light)

	light_type type = light_type::directional;
	depth_type depth = depth_type::invz;
	shadow_type shadow = shadow_type::hard;

	struct spot
	{
		void set_range(float r);
		float get_range() const
		{
			return range;
		}

		void set_outer_angle(float angle);
		float get_outer_angle() const
		{
			return outer_angle;
		}

		void set_inner_angle(float angle);
		float get_inner_angle() const
		{
			return inner_angle;
		}

		float range = 10.0f;
		float outer_angle = 60.0f;
		float inner_angle = 30.0f;
	};

	struct point
	{
		float range = 10.0f;
		float exponent_falloff = 1.0f;
		float fov_x_adjust = 0.0f;
		float fov_y_adjust = 0.0f;
		bool stencil_pack = true;
	};

	struct directional
	{
		float split_distribution = 0.6f;
		std::uint8_t num_splits = 4;
		bool stabilize = true;
	};

	spot spot_data;
	point point_data;
	directional directional_data;
	math::color color = {1.0f, 1.0f, 1.0f, 1.0f};
	float intensity = 1.0f;
};

#pragma once
#include <cstdint>
#include "program.h"
#include "core/reflection/rttr/rttr_enable.h"
#include "core/serialization/serialization.h"
#include "core/math/math_includes.h"

enum class light_type : std::uint8_t
{
	spot = 0,
	point = 1,
	directional = 2,

	Count
};

enum class depth_impl : std::uint8_t
{
	InvZ = 0,
	Linear = 1,

	Count
};

enum class pack_depth : std::uint8_t
{
	RGBA = 0,
	VSM = 1,

	Count
};

enum class sm_impl : std::uint8_t
{
	Hard = 0,
	PCF = 1,
	VSM = 2,
	ESM = 3,

	Count
};

enum class sm_type : std::uint8_t
{
	Single = 0,
	Omni = 1,
	Cascade = 2,

	Count
};


struct light
{
	REFLECTABLE(light)
	SERIALIZABLE(light)

	light_type light_type = light_type::directional;
	depth_impl depth_impl = depth_impl::InvZ;
	sm_impl sm_impl = sm_impl::Hard;

	struct spot
	{
		void set_range(float r);
		float get_range() const { return range; }

		void set_outer_angle(float angle);
		float get_outer_angle() const { return outer_angle; }

		void set_inner_angle(float angle);
		float get_inner_angle() const { return inner_angle; }

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
	math::color color = { 1.0f, 1.0f, 1.0f, 1.0f };
	float intensity = 1.0f;
};

#pragma once
#include "core/math/math_includes.h"
#include "inspector.h"

struct inspector_vec2 : public inspector
{
	REFLECTABLEV(inspector_vec2, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_vec2, math::vec2)

struct inspector_vec3 : public inspector
{
	REFLECTABLEV(inspector_vec3, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_vec3, math::vec3)

struct inspector_vec4 : public inspector
{
	REFLECTABLEV(inspector_vec4, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_vec4, math::vec4)

struct inspector_color : public inspector
{
	REFLECTABLEV(inspector_color, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_color, math::color)

struct inspector_quaternion : public inspector
{
	REFLECTABLEV(inspector_quaternion, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_quaternion, math::quat)

struct inspector_transform : public inspector
{
	REFLECTABLEV(inspector_transform, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_transform, math::transform)

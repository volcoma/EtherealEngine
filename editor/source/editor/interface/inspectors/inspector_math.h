#pragma once
#include "inspector.h"
#include "core/math/math_includes.h"

struct Inspector_Vector2 : public Inspector
{
	REFLECTABLE(Inspector_Vector2, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Vector2, math::vec2)

struct Inspector_Vector3 : public Inspector
{
	REFLECTABLE(Inspector_Vector3, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Vector3, math::vec3)

struct Inspector_Vector4 : public Inspector
{
	REFLECTABLE(Inspector_Vector4, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Vector4, math::vec4)

struct Inspector_Color : public Inspector
{
	REFLECTABLE(Inspector_Color, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Color, math::color)


struct Inspector_Quaternion : public Inspector
{
	REFLECTABLE(Inspector_Quaternion, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Quaternion, math::quat)


struct Inspector_Transform : public Inspector
{
	REFLECTABLE(Inspector_Transform, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Transform, math::transform_t)

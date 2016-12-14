#pragma once

#include "Inspector.h"
#include "Core/common/basetypes.hpp"

struct Inspector_iRect : public Inspector
{
	REFLECTABLE(Inspector_iRect, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_iRect, iRect)

struct Inspector_uRect : public Inspector
{
	REFLECTABLE(Inspector_uRect, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_uRect, uRect)

struct Inspector_fRect : public Inspector
{
	REFLECTABLE(Inspector_fRect, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_fRect, fRect)

struct Inspector_iPoint : public Inspector
{
	REFLECTABLE(Inspector_iPoint, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_iPoint, iPoint)

struct Inspector_uPoint : public Inspector
{
	REFLECTABLE(Inspector_uPoint, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_uPoint, uPoint)

struct Inspector_fPoint : public Inspector
{
	REFLECTABLE(Inspector_fPoint, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_fPoint, fPoint)

struct Inspector_iSize : public Inspector
{
	REFLECTABLE(Inspector_iSize, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_iSize, iSize)

struct Inspector_uSize : public Inspector
{
	REFLECTABLE(Inspector_uSize, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_uSize, uSize)

struct Inspector_fSize : public Inspector
{
	REFLECTABLE(Inspector_fSize, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_fSize, fSize)

struct Inspector_iRange : public Inspector
{
	REFLECTABLE(Inspector_iRange, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_iRange, iRange)

struct Inspector_uRange : public Inspector
{
	REFLECTABLE(Inspector_uRange, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_uRange, uRange)

struct Inspector_fRange : public Inspector
{
	REFLECTABLE(Inspector_fRange, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_fRange, fRange)
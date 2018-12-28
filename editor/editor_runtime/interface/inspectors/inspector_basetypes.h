#pragma once

#include "inspector.h"

#include <core/common/basetypes.hpp>

struct inspector_irect32_t : public inspector
{
	REFLECTABLEV(inspector_irect32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_irect32_t, irect32_t)

struct inspector_urect32_t : public inspector
{
	REFLECTABLEV(inspector_urect32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_urect32_t, urect32_t)

struct inspector_frect_t : public inspector
{
	REFLECTABLEV(inspector_frect_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_frect_t, frect_t)

struct inspector_ipoint32_t : public inspector
{
	REFLECTABLEV(inspector_ipoint32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_ipoint32_t, ipoint32_t)

struct inspector_upoint32_t : public inspector
{
	REFLECTABLEV(inspector_upoint32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_upoint32_t, upoint32_t)

struct inspector_fpoint_t : public inspector
{
	REFLECTABLEV(inspector_fpoint_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_fpoint_t, fpoint_t)

struct inspector_isize32_t : public inspector
{
	REFLECTABLEV(inspector_isize32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_isize32_t, isize32_t)

struct inspector_usize32_t : public inspector
{
	REFLECTABLEV(inspector_usize32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_usize32_t, usize32_t)

struct inspector_fsize_t : public inspector
{
	REFLECTABLEV(inspector_fsize_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_fsize_t, fsize_t)

struct inspector_irange32_t : public inspector
{
	REFLECTABLEV(inspector_irange32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_irange32_t, irange32_t)

struct inspector_urange32_t : public inspector
{
	REFLECTABLEV(inspector_urange32_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_urange32_t, urange32_t)

struct inspector_frange_t : public inspector
{
	REFLECTABLEV(inspector_frange_t, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_frange_t, frange_t)

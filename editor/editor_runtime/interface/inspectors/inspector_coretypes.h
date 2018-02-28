#pragma once

#include "inspector.h"
#include <string>
#include <chrono>

struct inspector_bool : public inspector
{
	REFLECTABLEV(inspector_bool, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_bool, bool)

struct inspector_float : public inspector
{
	REFLECTABLEV(inspector_float, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_float, float)

struct inspector_double : public inspector
{
	REFLECTABLEV(inspector_double, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_double, double)

struct inspector_int8 : public inspector
{
	REFLECTABLEV(inspector_int8, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_int8, std::int8_t)

struct inspector_int16 : public inspector
{
	REFLECTABLEV(inspector_int16, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_int16, std::int16_t)

struct inspector_int32 : public inspector
{
	REFLECTABLEV(inspector_int32, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_int32, std::int32_t)

struct inspector_int64 : public inspector
{
	REFLECTABLEV(inspector_int64, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_int64, std::int64_t)

struct inspector_uint8 : public inspector
{
	REFLECTABLEV(inspector_uint8, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_uint8, std::uint8_t)

struct inspector_uint16 : public inspector
{
	REFLECTABLEV(inspector_uint16, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_uint16, std::uint16_t)

struct inspector_uint32 : public inspector
{
	REFLECTABLEV(inspector_uint32, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_uint32, std::uint32_t)

struct inspector_uint64 : public inspector
{
	REFLECTABLEV(inspector_uint64, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_uint64, std::uint64_t)
struct inspector_string : public inspector
{
	REFLECTABLEV(inspector_string, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_string, std::string)

struct inspector_duration_sec_float : public inspector
{
	REFLECTABLEV(inspector_duration_sec_float, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_duration_sec_float, std::chrono::duration<float>)

struct inspector_duration_sec_double : public inspector
{
	REFLECTABLEV(inspector_duration_sec_double, inspector)
	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_duration_sec_double, std::chrono::duration<double>)

#pragma once

#include "inspector.h"

struct Inspector_Bool : public Inspector
{
	REFLECTABLE(Inspector_Bool, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};	
INSPECTOR_REFLECT(Inspector_Bool, bool)


struct Inspector_Float : public Inspector
{
	REFLECTABLE(Inspector_Float, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Float, float)


struct Inspector_Double : public Inspector
{
	REFLECTABLE(Inspector_Double, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Double, double)

struct Inspector_Int8 : public Inspector
{
	REFLECTABLE(Inspector_Int8, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Int8, std::int8_t)

struct Inspector_Int16 : public Inspector
{
	REFLECTABLE(Inspector_Int16, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Int16, std::int16_t)

struct Inspector_Int32 : public Inspector
{
	REFLECTABLE(Inspector_Int32, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Int32, std::int32_t)

struct Inspector_Int64 : public Inspector
{
	REFLECTABLE(Inspector_Int64, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_Int64, std::int64_t)

struct Inspector_UInt8 : public Inspector
{
	REFLECTABLE(Inspector_UInt8, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_UInt8, std::uint8_t)

struct Inspector_UInt16 : public Inspector
{
	REFLECTABLE(Inspector_UInt16, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_UInt16, std::uint16_t)

struct Inspector_UInt32 : public Inspector
{
	REFLECTABLE(Inspector_UInt32, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_UInt32, std::uint32_t)

struct Inspector_UInt64 : public Inspector
{
	REFLECTABLE(Inspector_UInt64, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_UInt64, std::uint64_t)


struct Inspector_String : public Inspector
{
	REFLECTABLE(Inspector_String, Inspector)
	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_String, std::string)
#pragma  once

#include "Inspector.h"

inline rttr::variant get_meta_empty(const rttr::variant& other)
{
	return rttr::variant();
}

bool inspectVar(rttr::variant& var, bool readOnly = false, std::function<rttr::variant(const rttr::variant&)> get_metadata = get_meta_empty);
bool inspectArray(rttr::variant& var, bool readOnly = false);
bool inspectEnum(rttr::variant& var, rttr::enumeration& data, bool readOnly = false);
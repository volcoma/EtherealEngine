#pragma  once

#include "inspector.h"

inline rttr::variant get_meta_empty(const rttr::variant& other) { return rttr::variant(); }
bool inspect_var(rttr::variant& var, bool readOnly = false, std::function<rttr::variant(const rttr::variant&)> get_metadata = get_meta_empty);
bool inspect_array(rttr::variant& var, bool readOnly = false);
bool inspect_associative_container(rttr::variant& var, bool readOnly = false);
bool inspect_enum(rttr::variant& var, rttr::enumeration& data, bool readOnly = false);

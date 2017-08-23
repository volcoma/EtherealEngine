#pragma once

#include "../gui_system.h"
#include "core/reflection/reflection.h"
#include "core/reflection/registration.h"

struct inspector
{
	REFLECTABLEV(inspector)

	virtual ~inspector() = default;

	virtual bool inspect(rttr::variant& var, bool read_only,
						 std::function<rttr::variant(const rttr::variant&)> get_metadata) = 0;
};

struct property_layout
{
	property_layout(const rttr::property& prop, bool columns = true);

	property_layout(const std::string& name, bool columns = true);

	property_layout(const std::string& name, const std::string& tooltip, bool columns = true);

	~property_layout();
};

REFLECT_EXTERN(inspector);

#define INSPECTED_TYPE "inspected_type"

#define INSPECTOR_REFLECT(inspector_type, inspected_type)                                                    \
	REFLECT_INLINE(inspector_type)                                                                           \
	{                                                                                                        \
		rttr::registration::class_<inspector_type>(#inspector_type)(                                         \
			rttr::metadata(INSPECTED_TYPE, rttr::type::get<inspected_type>()))                               \
			.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);                                         \
	}

#define DECLARE_INSPECTOR(inspector_type, inspected_type)                                                    \
	struct inspector_type : public inspector                                                                 \
	{                                                                                                        \
		REFLECTABLE(inspector_type, inspector)                                                               \
		bool inspect(rttr::variant& var, bool read_only,                                                      \
					 std::function<rttr::variant(const rttr::variant&)> get_metadata);                       \
	};                                                                                                       \
	INSPECTOR_REFLECT(inspector_type, inspected_type)

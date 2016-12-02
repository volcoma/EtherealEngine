#pragma once

#include "Core/reflection/reflection.h"
#include "../GUI.h"

struct Inspector
{
	REFLECTABLE(Inspector)

	virtual ~Inspector() = default;

	virtual bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata) = 0;
};


struct PropertyLayout
{
	PropertyLayout(const rttr::property& prop, bool columns = true);

	PropertyLayout(const std::string& name, bool columns = true);

	~PropertyLayout();
};


REFLECT(Inspector)
{
	rttr::registration::class_<Inspector>("Inspector");
}

#define INSPECTED_TYPE "inspected_type"

#define INSPECTOR_REFLECT(editorType, editType)						\
REFLECT(editorType)													\
{																	\
	rttr::registration::class_<editorType>(#editorType)				\
	(																\
		rttr::metadata(INSPECTED_TYPE, rttr::type::get<editType>())	\
	)																\
	.constructor<>()												\
	(																\
		rttr::policy::ctor::as_std_shared_ptr						\
	)																\
	;																\
}

#define DECLARE_INSPECTOR(inspector, type)									\
struct inspector : public Inspector											\
{																			\
	REFLECTABLE(Inspector_Bool, Inspector)									\
	bool inspect(rttr::variant& var,										\
		bool readOnly,														\
		std::function<rttr::variant(const rttr::variant&)> get_metadata);	\
};																			\
INSPECTOR_REFLECT(inspector, type)
#pragma once
#include "inspector.h"
#include "runtime/ecs/ecs.h"

struct Inspector_Entity : public Inspector
{
	REFLECTABLE(Inspector_Entity, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(Inspector_Entity, runtime::Entity)
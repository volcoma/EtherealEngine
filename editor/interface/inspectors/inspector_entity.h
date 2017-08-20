#pragma once
#include "inspector.h"
#include "runtime/ecs/ecs.h"

struct inspector_entity : public inspector
{

	REFLECTABLEV(inspector_entity, inspector)

	bool inspect(rttr::variant& var, bool readOnly,
				 std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(inspector_entity, runtime::entity)

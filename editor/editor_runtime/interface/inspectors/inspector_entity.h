#pragma once
#include "inspector.h"
#include "runtime/ecs/ecs.h"

struct inspector_entity : public inspector
{

	REFLECTABLEV(inspector_entity, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};

INSPECTOR_REFLECT(inspector_entity, runtime::entity)

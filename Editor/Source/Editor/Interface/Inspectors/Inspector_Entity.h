#pragma once
#include "Inspector.h"
#include "Runtime/Ecs/World.h"

struct Inspector_Entity : public Inspector
{
	REFLECTABLE(Inspector_Entity, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(Inspector_Entity, ecs::Entity)
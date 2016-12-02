#pragma once
#include "Inspector.h"
#include "Runtime/Ecs/Components/LightComponent.h"

struct Inspector_LightComponent : public Inspector
{
	REFLECTABLE(Inspector_LightComponent, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(Inspector_LightComponent, LightComponent)
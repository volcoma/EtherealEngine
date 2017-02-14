#pragma once
#include "inspector.h"
#include "runtime/ecs/components/light_component.h"

struct Inspector_LightComponent : public Inspector
{
	REFLECTABLE(Inspector_LightComponent, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(Inspector_LightComponent, LightComponent)

#include "runtime/ecs/components/reflection_probe_component.h"

struct Inspector_ReflectionProbeComponent : public Inspector
{
	REFLECTABLE(Inspector_ReflectionProbeComponent, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};

INSPECTOR_REFLECT(Inspector_ReflectionProbeComponent, ReflectionProbeComponent)
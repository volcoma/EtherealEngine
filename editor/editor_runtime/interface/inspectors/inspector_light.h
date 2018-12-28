#pragma once
#include "inspector.h"

#include <runtime/ecs/components/light_component.h>
#include <runtime/ecs/components/reflection_probe_component.h>

struct inspector_light_component : public inspector
{
	REFLECTABLEV(inspector_light_component, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};

INSPECTOR_REFLECT(inspector_light_component, light_component)


struct inspector_reflection_probe_component : public inspector
{
	REFLECTABLEV(inspector_reflection_probe_component, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};

INSPECTOR_REFLECT(inspector_reflection_probe_component, reflection_probe_component)

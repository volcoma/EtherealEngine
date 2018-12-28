#pragma once
#include "inspector.h"

#include <runtime/ecs/components/audio_source_component.h>

struct inspector_audio_source_component : public inspector
{
	REFLECTABLEV(inspector_audio_source_component, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};

INSPECTOR_REFLECT(inspector_audio_source_component, audio_source_component)

#pragma once
#include "../../../ecs/components/reflection_probe_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(reflection_probe_component)
{
	rttr::registration::class_<reflection_probe_component>("reflection_probe_component")(
		rttr::metadata("Category", "Lighting"), rttr::metadata("Id", "Reflection Probe"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

SAVE_EXTERN(reflection_probe_component);
LOAD_EXTERN(reflection_probe_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(reflection_probe_component)

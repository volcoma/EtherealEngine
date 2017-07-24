#pragma once
#include "../../../ecs/components/light_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(light_component)
{
	rttr::registration::class_<light_component>("light_component")(rttr::metadata("Category", "Lighting"),
																   rttr::metadata("Id", "Light"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

SAVE_EXTERN(light_component);
LOAD_EXTERN(light_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(light_component)

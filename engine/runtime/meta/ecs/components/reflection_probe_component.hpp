#pragma once
#include "../../../ecs/components/reflection_probe_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(reflection_probe_component);

SAVE_EXTERN(reflection_probe_component);
LOAD_EXTERN(reflection_probe_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(reflection_probe_component)

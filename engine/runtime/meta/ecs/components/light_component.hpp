#pragma once
#include "../../../ecs/components/light_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(light_component);
SAVE_EXTERN(light_component);
LOAD_EXTERN(light_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(light_component)

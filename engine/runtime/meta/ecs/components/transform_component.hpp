#pragma once

#include "../../../ecs/components/transform_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(transform_component);
SAVE_EXTERN(transform_component);
LOAD_EXTERN(transform_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(transform_component)

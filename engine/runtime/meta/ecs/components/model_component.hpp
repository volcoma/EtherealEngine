#pragma once
#include "../../../ecs/components/model_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(model_component);
SAVE_EXTERN(model_component);
LOAD_EXTERN(model_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(model_component)
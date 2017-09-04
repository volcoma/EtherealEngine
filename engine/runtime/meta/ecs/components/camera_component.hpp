#pragma once
#include "../../../ecs/components/camera_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(camera_component);
SAVE_EXTERN(camera_component);
LOAD_EXTERN(camera_component);

#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"
CEREAL_REGISTER_TYPE(camera_component)

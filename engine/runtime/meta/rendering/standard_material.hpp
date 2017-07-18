#pragma once
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "../../rendering/material.h"

REFLECT_EXTERN(standard_material);
SAVE_EXTERN(standard_material);
LOAD_EXTERN(standard_material);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(standard_material)
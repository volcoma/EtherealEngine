#pragma once
#include "../../../ecs/components/audio_source_component.h"
#include <core/reflection/reflection.h>
#include <core/serialization/serialization.h>

REFLECT_EXTERN(audio_source_component);
SAVE_EXTERN(audio_source_component);
LOAD_EXTERN(audio_source_component);

#include <core/serialization/associative_archive.h>
#include <core/serialization/binary_archive.h>
CEREAL_REGISTER_TYPE(audio_source_component)

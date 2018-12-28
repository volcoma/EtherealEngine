#pragma once
#include "../../../ecs/components/audio_listener_component.h"
#include <core/reflection/reflection.h>
#include <core/serialization/serialization.h>

REFLECT_EXTERN(audio_listener_component);
SAVE_EXTERN(audio_listener_component);
LOAD_EXTERN(audio_listener_component);

#include <core/serialization/associative_archive.h>
#include <core/serialization/binary_archive.h>
CEREAL_REGISTER_TYPE(audio_listener_component)

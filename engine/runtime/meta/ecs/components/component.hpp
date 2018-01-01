#pragma once
#include "../../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

namespace runtime
{
REFLECT_EXTERN(component);
SAVE_EXTERN(component);
LOAD_EXTERN(component);
}

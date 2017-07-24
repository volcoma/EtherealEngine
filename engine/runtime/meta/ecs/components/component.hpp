#pragma once
#include "../../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT_EXTERN(runtime::component);

namespace runtime
{
SAVE_EXTERN(component);
LOAD_EXTERN(component);
}

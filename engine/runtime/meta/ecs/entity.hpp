#pragma once
#include "../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

namespace runtime
{

std::map<std::uint64_t, runtime::entity>& get_serialization_map();

SAVE_EXTERN(entity);
LOAD_EXTERN(entity);
}

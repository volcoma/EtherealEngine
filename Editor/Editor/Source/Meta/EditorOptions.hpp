#pragma once

#include "Core/reflection/reflection.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/deque.hpp"
#include "Core/serialization/cereal/types/string.hpp"

SAVE(EditorOptions)
{
	ar(
		cereal::make_nvp("recent_projects", obj.recentProjects)
	);
}

LOAD(EditorOptions)
{
	ar(
		cereal::make_nvp("recent_projects", obj.recentProjects)
	);
}

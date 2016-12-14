#pragma once

#include "../EditorOptions.h"
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

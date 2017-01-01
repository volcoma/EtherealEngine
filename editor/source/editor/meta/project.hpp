#pragma once

#include "../project.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/deque.hpp"
#include "core/serialization/cereal/types/string.hpp"

namespace editor
{
	SAVE(ProjectManager::Options)
	{
		ar(
			cereal::make_nvp("recent_projects", obj.recent_project_paths)
		);
	}

	LOAD(ProjectManager::Options)
	{
		ar(
			cereal::make_nvp("recent_projects", obj.recent_project_paths)
		);
	}
}
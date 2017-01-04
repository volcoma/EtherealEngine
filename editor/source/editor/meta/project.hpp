#pragma once

#include "../project.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/deque.hpp"
#include "core/serialization/cereal/types/string.hpp"
#include "core/logging/logging.h"

namespace editor
{
	SAVE(ProjectManager::Options)
	{
		try_save(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
	}

	LOAD(ProjectManager::Options)
	{
		try_load(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
	}
}
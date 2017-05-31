#pragma once

#include "../system/project_manager.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/deque.hpp"
#include "core/serialization/types/string.hpp"
#include "core/logging/logging.h"

namespace editor
{
	SAVE(project_manager::options)
	{
		try_save(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
	}

	LOAD(project_manager::options)
	{
		try_load(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
	}
}
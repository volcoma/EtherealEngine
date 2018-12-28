#include "project_manager.hpp"

#include <core/serialization/associative_archive.h>
#include <core/serialization/types/deque.hpp>
#include <core/serialization/types/string.hpp>

namespace editor
{
SAVE(project_manager::options)
{
	try_save(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
}
SAVE_INSTANTIATE(project_manager::options, cereal::oarchive_associative_t);

LOAD(project_manager::options)
{
	try_load(ar, cereal::make_nvp("recent_projects", obj.recent_project_paths));
}
LOAD_INSTANTIATE(project_manager::options, cereal::iarchive_associative_t);
}

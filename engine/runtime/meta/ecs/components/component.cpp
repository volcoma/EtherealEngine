#include "component.hpp"
#include "../entity.hpp"
#include "core/serialization/associative_archive.h"

REFLECT(runtime::component)
{
	rttr::registration::class_<runtime::component>("component");
}

namespace runtime
{
SAVE(component)
{
	try_save(ar, cereal::make_nvp("owner", obj._entity));
}
SAVE_INSTANTIATE(component, cereal::oarchive_associative_t);

LOAD(component)
{
	try_load(ar, cereal::make_nvp("owner", obj._entity));
}
LOAD_INSTANTIATE(component, cereal::iarchive_associative_t);
}

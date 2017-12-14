#include "component.hpp"
#include "../entity.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/binary_archive.h"


namespace runtime
{
REFLECT(component)
{
	rttr::registration::class_<component>("component");
}

SAVE(component)
{
	try_save(ar, cereal::make_nvp("owner", obj._entity));
}
SAVE_INSTANTIATE(component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(component, cereal::oarchive_binary_t);

LOAD(component)
{
	try_load(ar, cereal::make_nvp("owner", obj._entity));

	obj.touch();
}
LOAD_INSTANTIATE(component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(component, cereal::iarchive_binary_t);
}

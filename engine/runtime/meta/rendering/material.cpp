#include "material.hpp"
#include "core/serialization/associative_archive.h"

SAVE(material)
{
	try_save(ar, cereal::make_nvp("cull_type", obj._cull_type));
}
SAVE_INSTANTIATE(material, cereal::oarchive_associative_t);

LOAD(material)
{
	try_load(ar, cereal::make_nvp("cull_type", obj._cull_type));
}
LOAD_INSTANTIATE(material, cereal::iarchive_associative_t);

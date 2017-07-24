#include "model_component.hpp"
#include "../../rendering/material.hpp"
#include "../../rendering/mesh.hpp"
#include "../../rendering/model.hpp"
#include "component.hpp"
#include "core/serialization/types/vector.hpp"

SAVE(model_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("static", obj._static));
	try_save(ar, cereal::make_nvp("casts_shadow", obj._casts_shadow));
	try_save(ar, cereal::make_nvp("casts_reflection", obj._casts_reflection));
	try_save(ar, cereal::make_nvp("model", obj._model));
}
SAVE_INSTANTIATE(model_component, cereal::oarchive_associative_t);

LOAD(model_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("static", obj._static));
	try_load(ar, cereal::make_nvp("casts_shadow", obj._casts_shadow));
	try_load(ar, cereal::make_nvp("casts_reflection", obj._casts_reflection));
	try_load(ar, cereal::make_nvp("model", obj._model));
}
LOAD_INSTANTIATE(model_component, cereal::iarchive_associative_t);

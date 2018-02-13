#include "model_component.hpp"
#include "../../rendering/material.hpp"
#include "../../rendering/mesh.hpp"
#include "../../rendering/model.hpp"
#include "component.hpp"
#include "core/serialization/types/vector.hpp"

REFLECT(model_component)
{
	rttr::registration::class_<model_component>("model_component")(rttr::metadata("category", "RENDERING"),
																   rttr::metadata("pretty_name", "Model"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("static", &model_component::is_static,
				  &model_component::set_static)(rttr::metadata("pretty_name", "Static"))
		.property("casts_shadow", &model_component::casts_shadow,
				  &model_component::set_casts_shadow)(rttr::metadata("pretty_name", "Casts Shadow"))
		.property("casts_reflection", &model_component::casts_reflection,
				  &model_component::set_casts_reflection)(rttr::metadata("pretty_name", "Casts Reflection"))
		.property("model", &model_component::get_model,
				  &model_component::set_model)(rttr::metadata("pretty_name", "Model"));
}

SAVE(model_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("static", obj.static_));
	try_save(ar, cereal::make_nvp("casts_shadow", obj.casts_shadow_));
	try_save(ar, cereal::make_nvp("casts_reflection", obj.casts_reflection_));
	try_save(ar, cereal::make_nvp("model", obj.model_));
	try_save(ar, cereal::make_nvp("bone_entities", obj.bone_entities_));
}
SAVE_INSTANTIATE(model_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(model_component, cereal::oarchive_binary_t);

LOAD(model_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("static", obj.static_));
	try_load(ar, cereal::make_nvp("casts_shadow", obj.casts_shadow_));
	try_load(ar, cereal::make_nvp("casts_reflection", obj.casts_reflection_));
	try_load(ar, cereal::make_nvp("model", obj.model_));
	try_load(ar, cereal::make_nvp("bone_entities", obj.bone_entities_));
}
LOAD_INSTANTIATE(model_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(model_component, cereal::iarchive_binary_t);

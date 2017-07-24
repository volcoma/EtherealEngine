#include "model_component.hpp"
#include "../../rendering/model.hpp"
#include "component.hpp"
#include "core/serialization/types/vector.hpp"

REFLECT(model_component)
{
	rttr::registration::class_<model_component>("model_component")(rttr::metadata("Category", "Rendering"),
																   rttr::metadata("Id", "Model"))
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

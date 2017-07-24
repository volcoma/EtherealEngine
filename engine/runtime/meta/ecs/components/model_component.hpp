#pragma once
#include "../../../ecs/components/model_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

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

SAVE_EXTERN(model_component);
LOAD_EXTERN(model_component);

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(model_component)

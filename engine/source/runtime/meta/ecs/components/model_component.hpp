#pragma once
#include "../../../ecs/components/model_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/logging/logging.h"

REFLECT(ModelComponent)
{
	rttr::registration::class_<ModelComponent>("ModelComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Static",
			&ModelComponent::is_static,
			&ModelComponent::set_static)
		.property("Casts Shadow",
			&ModelComponent::casts_shadow,
			&ModelComponent::set_casts_shadow)
		.property("Casts Reflection",
			&ModelComponent::casts_reflection,
			&ModelComponent::set_casts_reflection)
		.property("Model",
			&ModelComponent::get_model,
			&ModelComponent::set_model)
		;
}


SAVE(ModelComponent)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_save(ar, cereal::make_nvp("static", obj._static));
	try_save(ar, cereal::make_nvp("casts_shadow", obj._casts_shadow));
	try_save(ar, cereal::make_nvp("casts_reflection", obj._casts_reflection));
	try_save(ar, cereal::make_nvp("model", obj._model));
}


LOAD(ModelComponent)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_load(ar, cereal::make_nvp("static", obj._static));
	try_load(ar, cereal::make_nvp("casts_shadow", obj._casts_shadow));
	try_load(ar, cereal::make_nvp("casts_reflection", obj._casts_reflection));
	try_load(ar, cereal::make_nvp("model", obj._model));
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(ModelComponent);
#pragma once
#include "../../../ecs/components/model_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"

REFLECT(ModelComponent)
{
	rttr::registration::class_<ModelComponent>("ModelComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Static",
			&ModelComponent::isStatic,
			&ModelComponent::setStatic)
		.property("Casts Shadow",
			&ModelComponent::castsShadow,
			&ModelComponent::setCastShadow)
		.property("Casts Reflection",
			&ModelComponent::castsReflection,
			&ModelComponent::setCastReflelction)
		.property("Model",
			&ModelComponent::getModel,
			&ModelComponent::setModel)
		;
}


SAVE(ModelComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj)),
		cereal::make_nvp("static", obj.mStatic),
		cereal::make_nvp("casts_shadow", obj.mCastShadow),
		cereal::make_nvp("casts_reflection", obj.mCastReflection),
		cereal::make_nvp("model", obj.mModel)
	);
}


LOAD(ModelComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj)),
		cereal::make_nvp("static", obj.mStatic),
		cereal::make_nvp("casts_shadow", obj.mCastShadow),
		cereal::make_nvp("casts_reflection", obj.mCastReflection),
		cereal::make_nvp("model", obj.mModel)
	);
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(ModelComponent);
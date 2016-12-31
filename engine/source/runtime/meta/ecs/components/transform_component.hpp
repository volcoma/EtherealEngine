#pragma once
#include "../../../ecs/components/transform_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"

REFLECT(TransformComponent)
{
	rttr::registration::class_<TransformComponent>("TransformComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Local",
			&TransformComponent::getLocalTransform,
			&TransformComponent::setLocalTransform)
		(
			rttr::metadata("Tooltip", "This is the local transformation. It is relative to the parent.")
		)
		.property("World",
			&TransformComponent::getTransform,
			&TransformComponent::setTransform)
		(
			rttr::metadata("Tooltip", "This is the world transformation. Affected by parent transformation.")
		)
		.property("Slow Parenting",
			&TransformComponent::getSlowParenting,
			&TransformComponent::setSlowParenting)
		(
			rttr::metadata("Tooltip", "Enables/disables slow parenting.")
		)
		.property("Slow Parenting Speed",
			&TransformComponent::getSlowParentingSpeed,
			&TransformComponent::setSlowParentingSpeed)
		(
			rttr::metadata("Tooltip", "Controls the speed at which the slow parenting works."),
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 30.0f)
		)
		;

}

SAVE(TransformComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj)),
		cereal::make_nvp("local_transform", obj.mLocalTransform),
		cereal::make_nvp("children", obj.mChildren),
		cereal::make_nvp("slow_parenting", obj.mSlowParenting),
		cereal::make_nvp("slow_parenting_speed", obj.mSlowParentingSpeed)
		);

}

LOAD(TransformComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj)),
		cereal::make_nvp("local_transform", obj.mLocalTransform),
		cereal::make_nvp("children", obj.mChildren),
		cereal::make_nvp("slow_parenting", obj.mSlowParenting),
		cereal::make_nvp("slow_parenting_speed", obj.mSlowParentingSpeed)
		);

	auto thisHandle = obj.handle();
	for (auto child : obj.mChildren)
	{
		child.lock()->mParent = thisHandle;
	}
}

#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(TransformComponent);
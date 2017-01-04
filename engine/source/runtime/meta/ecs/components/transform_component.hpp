#pragma once
#include "../../../ecs/components/transform_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/logging/logging.h"

REFLECT(TransformComponent)
{
	rttr::registration::class_<TransformComponent>("TransformComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		.property("Local",
			&TransformComponent::get_local_transform,
			&TransformComponent::set_local_transform)
		(
			rttr::metadata("Tooltip", "This is the local transformation. It is relative to the parent.")
		)
		.property("World",
			&TransformComponent::get_transform,
			&TransformComponent::set_transform)
		(
			rttr::metadata("Tooltip", "This is the world transformation. Affected by parent transformation.")
		)
		.property("Slow Parenting",
			&TransformComponent::get_slow_parenting,
			&TransformComponent::set_slow_parenting)
		(
			rttr::metadata("Tooltip", "Enables/disables slow parenting.")
		)
		.property("Slow Parenting Speed",
			&TransformComponent::get_slow_parenting_speed,
			&TransformComponent::set_slow_parenting_speed)
		(
			rttr::metadata("Tooltip", "Controls the speed at which the slow parenting works."),
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 30.0f)
		)
		;

}

SAVE(TransformComponent)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_save(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_save(ar, cereal::make_nvp("children", obj._children));
	try_save(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_save(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));
}

LOAD(TransformComponent)
{	
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_load(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_load(ar, cereal::make_nvp("children", obj._children));
	try_load(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_load(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));
		
	auto thisHandle = obj.handle();
	for (auto child : obj._children)
	{
		child.lock()->_parent = thisHandle;
	}
}

#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(TransformComponent);
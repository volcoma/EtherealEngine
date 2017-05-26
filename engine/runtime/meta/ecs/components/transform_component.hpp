#pragma once
#include "../../../ecs/components/transform_component.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/logging/logging.h"

REFLECT(transform_component)
{
	rttr::registration::class_<transform_component>("transform_component")
		(
			rttr::metadata("Id", "Transform")
		)
		.constructor<>()
		(		
			rttr::policy::ctor::as_std_shared_ptr
		)
		.property("local",
			&transform_component::get_local_transform,
			&transform_component::set_local_transform)
		(
			rttr::metadata("pretty_name", "Local"),
			rttr::metadata("Tooltip", "This is the local transformation. It is relative to the parent.")
		)
		.property("world",
			&transform_component::get_transform,
			&transform_component::set_transform)
		(
			rttr::metadata("pretty_name", "World"),
			rttr::metadata("Tooltip", "This is the world transformation. Affected by parent transformation.")
		)
		.property("slow_parenting",
			&transform_component::get_slow_parenting,
			&transform_component::set_slow_parenting)
		(
			rttr::metadata("pretty_name", "Slow Parenting"),
			rttr::metadata("Tooltip", "Enables/disables slow parenting.")
		)
		.property("slow_parenting_speed",
			&transform_component::get_slow_parenting_speed,
			&transform_component::set_slow_parenting_speed)
		(
			rttr::metadata("pretty_name", "Slow Parenting Speed"),
			rttr::metadata("Tooltip", "Controls the speed at which the slow parenting works."),
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 30.0f)
		)
		;

}

SAVE(transform_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_save(ar, cereal::make_nvp("children", obj._children));
	try_save(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_save(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));
}

LOAD(transform_component)
{	
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_load(ar, cereal::make_nvp("children", obj._children));
	try_load(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_load(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));
		
	auto handle = obj.handle();
	for (auto child : obj._children)
	{
		child.lock()->_parent = handle;
	}
}

#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(transform_component);
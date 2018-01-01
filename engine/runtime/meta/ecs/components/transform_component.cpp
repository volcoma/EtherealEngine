#include "transform_component.hpp"
#include "../../core/math/transform.hpp"
#include "../../core/math/vector.hpp"
#include "component.hpp"
#include "core/serialization/types/vector.hpp"

REFLECT(transform_component)
{
	rttr::registration::class_<transform_component>("transform_component")(
		rttr::metadata("category", "RENDERING"), rttr::metadata("pretty_name", "Transform"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("local", &transform_component::get_local_transform,
				  &transform_component::set_local_transform)(
			rttr::metadata("pretty_name", "Local"),
			rttr::metadata("tooltip", "This is the local transformation. It is "
									  "relative to the parent."))
		.property("world", &transform_component::get_transform, &transform_component::set_transform)(
			rttr::metadata("pretty_name", "World"),
			rttr::metadata("tooltip", "This is the world transformation. "
									  "Affected by parent transformation."));
}

SAVE(transform_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_save(ar, cereal::make_nvp("children", obj._children));
}
SAVE_INSTANTIATE(transform_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(transform_component, cereal::oarchive_binary_t);

LOAD(transform_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_load(ar, cereal::make_nvp("children", obj._children));

	for(auto child : obj._children)
	{
		if(child.valid())
		{
			auto child_transform = child.get_component<transform_component>().lock();
			if(child_transform)
			{
				child_transform->_parent = obj.get_entity();
			}
		}
	}
	obj.resolve();
}
LOAD_INSTANTIATE(transform_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(transform_component, cereal::iarchive_binary_t);

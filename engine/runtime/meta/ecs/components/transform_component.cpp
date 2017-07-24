#include "transform_component.hpp"
#include "component.hpp"
#include "core/meta/math/transform.hpp"
#include "core/meta/math/vector.hpp"
#include "core/serialization/types/vector.hpp"

SAVE(transform_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_save(ar, cereal::make_nvp("children", obj._children));
	try_save(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_save(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));
}
SAVE_INSTANTIATE(transform_component, cereal::oarchive_associative_t);

LOAD(transform_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("local_transform", obj._local_transform));
	try_load(ar, cereal::make_nvp("children", obj._children));
	try_load(ar, cereal::make_nvp("slow_parenting", obj._slow_parenting));
	try_load(ar, cereal::make_nvp("slow_parenting_speed", obj._slow_parenting_speed));

	auto handle = obj.handle();
	for(auto child : obj._children)
	{
		child.lock()->_parent = handle;
	}
}
LOAD_INSTANTIATE(transform_component, cereal::iarchive_associative_t);

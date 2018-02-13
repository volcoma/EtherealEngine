#include "light_component.hpp"

#include "../../core/math/vector.hpp"
#include "../../rendering/light.hpp"
#include "component.hpp"

REFLECT(light_component)
{
	rttr::registration::class_<light_component>("light_component")(rttr::metadata("category", "LIGHTING"),
																   rttr::metadata("pretty_name", "Light"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

SAVE(light_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("light", obj.light_));
}
SAVE_INSTANTIATE(light_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(light_component, cereal::oarchive_binary_t);

LOAD(light_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("light", obj.light_));
}
LOAD_INSTANTIATE(light_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(light_component, cereal::iarchive_binary_t);

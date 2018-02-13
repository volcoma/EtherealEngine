#include "reflection_probe_component.hpp"
#include "../../core/math/vector.hpp"
#include "../../rendering/reflection_probe.hpp"
#include "component.hpp"

REFLECT(reflection_probe_component)
{
	rttr::registration::class_<reflection_probe_component>("reflection_probe_component")(
		rttr::metadata("category", "LIGHTING"), rttr::metadata("pretty_name", "Reflection Probe"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

SAVE(reflection_probe_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("probe", obj.probe_));
}
SAVE_INSTANTIATE(reflection_probe_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(reflection_probe_component, cereal::oarchive_binary_t);

LOAD(reflection_probe_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("probe", obj.probe_));
}
LOAD_INSTANTIATE(reflection_probe_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(reflection_probe_component, cereal::iarchive_binary_t);

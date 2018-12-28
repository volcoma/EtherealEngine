#include "audio_listener_component.hpp"
#include "component.hpp"

#include "../../assets/asset_handle.hpp"
#include "../../core/common/basetypes.hpp"

#include <core/serialization/types/vector.hpp>

REFLECT(audio_listener_component)
{
	rttr::registration::class_<audio_listener_component>("audio_listener_component")(
		rttr::metadata("category", "AUDIO"), rttr::metadata("pretty_name", "Audio Listener"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);
}

SAVE(audio_listener_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
}
SAVE_INSTANTIATE(audio_listener_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(audio_listener_component, cereal::oarchive_binary_t);

LOAD(audio_listener_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
}
LOAD_INSTANTIATE(audio_listener_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(audio_listener_component, cereal::iarchive_binary_t);

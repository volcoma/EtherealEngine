#include "audio_source_component.hpp"
#include "../../assets/asset_handle.hpp"
#include "component.hpp"
#include "core/meta/common/basetypes.hpp"
#include "core/serialization/types/vector.hpp"

REFLECT(audio_source_component)
{
	rttr::registration::class_<audio_source_component>("audio_source_component")(
		rttr::metadata("category", "AUDIO"), rttr::metadata("pretty_name", "Audio Source"))
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("auto_play", &audio_source_component::get_autoplay,
				  &audio_source_component::set_autoplay)(rttr::metadata("pretty_name", "Auto Play"))
		.property("loop", &audio_source_component::is_looping,
				  &audio_source_component::set_loop)(rttr::metadata("pretty_name", "Loop"))
		.property("volume", &audio_source_component::get_volume, &audio_source_component::set_volume)(
			rttr::metadata("pretty_name", "Volume"), rttr::metadata("min", 0.0f), rttr::metadata("max", 1.0f))
		.property("pitch", &audio_source_component::get_pitch, &audio_source_component::set_pitch)(
			rttr::metadata("pretty_name", "Pitch"),
			rttr::metadata("tooltip", "A multiplier for the frequency (sample rate) of the source's buffer."),
			rttr::metadata("min", 0.5f), rttr::metadata("max", 2.0f))
		.property("volume_rolloff", &audio_source_component::get_volume_rolloff,
				  &audio_source_component::set_volume_rolloff)(
			rttr::metadata("pretty_name", "Volume Rolloff"), rttr::metadata("min", 0.0f),
			rttr::metadata("max", 10.0f))
		.property("range", &audio_source_component::get_range,
				  &audio_source_component::set_range)(rttr::metadata("pretty_name", "Range"))
		.property("sound", &audio_source_component::get_sound,
				  &audio_source_component::set_sound)(rttr::metadata("pretty_name", "Sound"));
	;
}

SAVE(audio_source_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("auto_play", obj._auto_play));
	try_save(ar, cereal::make_nvp("loop", obj._loop));
	try_save(ar, cereal::make_nvp("volume", obj._volume));
	try_save(ar, cereal::make_nvp("pitch", obj._pitch));
	try_save(ar, cereal::make_nvp("volume_rolloff", obj._volume_rolloff));
	try_save(ar, cereal::make_nvp("range", obj._range));
	try_save(ar, cereal::make_nvp("sound", obj._sound));
}
SAVE_INSTANTIATE(audio_source_component, cereal::oarchive_associative_t);
SAVE_INSTANTIATE(audio_source_component, cereal::oarchive_binary_t);

LOAD(audio_source_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("auto_play", obj._auto_play));
	try_load(ar, cereal::make_nvp("loop", obj._loop));
	try_load(ar, cereal::make_nvp("volume", obj._volume));
	try_load(ar, cereal::make_nvp("pitch", obj._pitch));
	try_load(ar, cereal::make_nvp("volume_rolloff", obj._volume_rolloff));
	try_load(ar, cereal::make_nvp("range", obj._range));
	try_load(ar, cereal::make_nvp("sound", obj._sound));

	obj.apply_all();
}
LOAD_INSTANTIATE(audio_source_component, cereal::iarchive_associative_t);
LOAD_INSTANTIATE(audio_source_component, cereal::iarchive_binary_t);

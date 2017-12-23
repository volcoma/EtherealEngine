#include "sound.hpp"
#include "core/serialization/binary_archive.h"
#include "core/serialization/types/chrono.hpp"

namespace audio
{

REFLECT(sound_data)
{
	rttr::registration::class_<sound_data>("sound_data")
		.property_readonly("sample_rate", &sound_data::sample_rate)(rttr::metadata("pretty_name", "Sample rate"),
															  rttr::metadata("tooltip", "Sample rate."))
		.property_readonly("channels", &sound_data::channels)(
			rttr::metadata("pretty_name", "Channels"), rttr::metadata("tooltip", "Mono or Stereo."))
		.property_readonly("duration", &sound_data::get_duration_as_double)(rttr::metadata("pretty_name", "Duration"),
															rttr::metadata("tooltip", "Duration in seconds."));
}

SAVE(sound_data)
{
	try_save(ar, cereal::make_nvp("data", obj.data));
	try_save(ar, cereal::make_nvp("sample_rate", obj.sample_rate));
	try_save(ar, cereal::make_nvp("channels", obj.channels));
	try_save(ar, cereal::make_nvp("duration", obj.duration));

}
SAVE_INSTANTIATE(sound_data, cereal::oarchive_binary_t);

LOAD(sound_data)
{
	try_load(ar, cereal::make_nvp("data", obj.data));
	try_load(ar, cereal::make_nvp("sample_rate", obj.sample_rate));
	try_load(ar, cereal::make_nvp("channels", obj.channels));
	try_load(ar, cereal::make_nvp("duration", obj.duration));
}
LOAD_INSTANTIATE(sound_data, cereal::iarchive_binary_t);
}

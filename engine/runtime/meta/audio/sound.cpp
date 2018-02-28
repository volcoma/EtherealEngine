#include "sound.hpp"
#include "core/serialization/binary_archive.h"
#include "core/serialization/types/chrono.hpp"

namespace audio
{

REFLECT(sound_info)
{
	rttr::registration::class_<sound_info>("sound_info")
		.property_readonly("bytes_per_sample", &sound_info::bytes_per_sample)(
			rttr::metadata("pretty_name", "Bytes per sample"), rttr::metadata("tooltip", "Bit depth."))
		.property_readonly("sample_rate", &sound_info::sample_rate)(
			rttr::metadata("pretty_name", "Sample rate"), rttr::metadata("tooltip", "Sample rate."))
		.property_readonly("channels", &sound_info::channels)(rttr::metadata("pretty_name", "Channels"),
															  rttr::metadata("tooltip", "Mono or Stereo."))
		.property_readonly("duration", &sound_info::duration)(
			rttr::metadata("pretty_name", "Duration"), rttr::metadata("tooltip", "Duration in seconds."));
}

SAVE(sound_info)
{
	try_save(ar, cereal::make_nvp("bytes_per_sample", obj.bytes_per_sample));
	try_save(ar, cereal::make_nvp("sample_rate", obj.sample_rate));
	try_save(ar, cereal::make_nvp("channels", obj.channels));
	try_save(ar, cereal::make_nvp("duration", obj.duration));
}
SAVE_INSTANTIATE(sound_info, cereal::oarchive_binary_t);

LOAD(sound_info)
{
	try_load(ar, cereal::make_nvp("bytes_per_sample", obj.bytes_per_sample));
	try_load(ar, cereal::make_nvp("sample_rate", obj.sample_rate));
	try_load(ar, cereal::make_nvp("channels", obj.channels));
	try_load(ar, cereal::make_nvp("duration", obj.duration));
}
LOAD_INSTANTIATE(sound_info, cereal::iarchive_binary_t);

SAVE(sound_data)
{
	try_save(ar, cereal::make_nvp("info", obj.info));
	try_save(ar, cereal::make_nvp("data", obj.data));
}
SAVE_INSTANTIATE(sound_data, cereal::oarchive_binary_t);

LOAD(sound_data)
{
	try_load(ar, cereal::make_nvp("info", obj.info));
	try_load(ar, cereal::make_nvp("data", obj.data));
}
LOAD_INSTANTIATE(sound_data, cereal::iarchive_binary_t);
}

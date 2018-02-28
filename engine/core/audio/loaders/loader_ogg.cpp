#include "../logger.h"
#include "loader.h"
#include "stb_vorbis.h"

namespace audio
{

bool load_ogg_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err)
{
	if(!data)
	{
		err = "ERROR : No data to load from.";
		return false;
	}
	if(!data_size)
	{
		err = "ERROR : No data to load from.";
		return false;
	}

	int vorb_err = 0;
	auto* oss = stb_vorbis_open_memory(data, static_cast<int>(data_size), &vorb_err, nullptr);

	if(!oss)
	{
		STBVorbisError decoded_err = STBVorbisError(vorb_err);
		err = "ERROR : Vorbis error code : " + std::to_string(decoded_err);
		return false;
	}
	stb_vorbis_info info = stb_vorbis_get_info(oss);
	result.info.channels = std::uint32_t(info.channels);
	result.info.sample_rate = info.sample_rate;
	result.info.bytes_per_sample = sizeof(std::int16_t);

	auto stream_len_in_samples = std::size_t(stb_vorbis_stream_length_in_samples(oss));

	std::size_t data_bytes =
		(stream_len_in_samples * std::size_t(info.channels)) * result.info.bytes_per_sample;

	float seconds = stb_vorbis_stream_length_in_seconds(oss);

	result.info.duration = sound_info::seconds_t(seconds);
	result.data.resize(data_bytes, 0);

	stb_vorbis_get_samples_short_interleaved(
		oss, info.channels, reinterpret_cast<std::int16_t*>(result.data.data()), int(data_bytes));

	stb_vorbis_close(oss);
	return true;
}
}

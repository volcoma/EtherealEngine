#include "../logger.h"
#include "loader.h"
#include <cstring>
namespace audio
{
/// Wave files have a master RIFF chunk which includes a
/// WAVE identifier followed by sub-chunks.
/// The data is stored in little-endian byte order.
struct wav_header
{
	//------------------
	// RIFF Header
	//------------------
	/// Contains "RIFF"
	char riff_header[4] = {0};

	/// Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
	std::int32_t wav_size = 0;

	/// Contains "WAVE"
	char wave_header[4] = {0};

	//------------------
	// Format Header
	//------------------

	/// Contains "fmt " (includes trailing space)
	char fmt_header[4] = {0};

	/// Should be 16 for PCM
	std::int32_t fmt_chunk_size = 0;

	/// Should be 1 for PCM. 3 for IEEE Float
	std::int16_t audio_format = 0;

	std::int16_t num_channels = 0;
	std::int32_t sample_rate = 0;

	/// Number of bytes per second. sample_rate * num_channels * bytes per sample
	std::int32_t byte_rate = 0;

	/// num_channels * bytes per sample
	std::int16_t sample_alignment = 0;

	/// Number of bits per sample
	std::int16_t bit_depth = 0;

	//------------------
	// Data Header
	//------------------
	/// Contains "data"
	char data_header[4] = {0};
	/// Number of bytes in data. Number of samples * num_channels * sample byte size
	std::int32_t data_bytes = 0;
};

bool load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
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
	constexpr static const size_t WAV_BIT_DEPTH = 16;

	wav_header header;
	std::memcpy(&header, data, sizeof(wav_header));

	if(std::memcmp(header.riff_header, "RIFF", 4) != 0)
	{
		err = "ERROR : Bad RIFF header.";
		return false;
	}

	if(std::memcmp(header.wave_header, "WAVE", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	if(std::memcmp(header.fmt_header, "fmt ", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	if(std::memcmp(header.data_header, "data", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	if(header.bit_depth != WAV_BIT_DEPTH)
	{
		err = "ERROR: This file is not 16 bit wav format.";
		return false;
	}

	result.sample_rate = std::uint32_t(header.sample_rate);
	result.duration = sound_data::duration_t(
		header.wav_size / (header.num_channels * header.sample_rate * (header.bit_depth / 8.0f)) * 1.0f);

	result.data.resize(std::size_t(header.data_bytes));
	result.bytes_per_sample = std::uint8_t(header.bit_depth) / 8;
	std::memcpy(result.data.data(), data + sizeof(wav_header), result.data.size());
	result.channels = std::uint32_t(header.num_channels);

	return true;
}
}

#include "../logger.h"
#include "loader.h"
#include <algorithm>
#include <cstring>
namespace audio
{

namespace detail
{
template <class T>
static void endian_swap(T* objp)
{
	auto memp = reinterpret_cast<std::uint8_t*>(objp);
	std::reverse(memp, memp + sizeof(T));
}
}

struct riff_header
{
	constexpr static const std::size_t spec_sz = 12;
	/// Contains the letters "RIFF" in ASCII form
	/// (0x52494646 big-endian form).
	char header[4] = {0};

	/// Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
	std::int32_t wav_size = 0;

	/// Contains the letters "WAVE"
	/// (0x57415645 big-endian form).
	char wave_header[4] = {0};
};

struct format_header
{
	constexpr static const std::size_t spec_sz = 24;
	/// Contains the letters "fmt " (includes trailing space)
	/// (0x666d7420 big-endian form).
	char header[4] = {0};

	/// 16 for PCM.  This is the size of the
	/// rest of the Subchunk which follows this number.
	/// May be different than 16. This indicates where
	/// the data header starts offsetted from here.
	std::int32_t fmt_chunk_size = 0;

	/// PCM = 1 (i.e. Linear quantization)
	/// Values other than 1 indicate some
	/// form of compression.
	std::int16_t audio_format = 0;

	/// Mono = 1, Stereo = 2, etc.
	std::int16_t num_channels = 0;

	/// 8000, 44100, etc.
	std::int32_t sample_rate = 0;

	/// Number of bytes per second. sample_rate * num_channels * bytes per sample
	std::int32_t byte_rate = 0;

	/// num_channels * bytes per sample
	std::int16_t sample_alignment = 0;

	/// Number of bits per sample
	std::int16_t bit_depth = 0;
};

struct data_header
{
	constexpr static const std::size_t spec_sz = 8;
	/// Contains the letters "data"
	/// (0x64617461 big-endian form).
	char header[4] = {0};

	/// Number of bytes in data. Number of samples * num_channels * sample byte size
	std::int32_t data_bytes = 0;
};

/// Wave files have a master RIFF chunk which includes a
/// WAVE identifier followed by sub-chunks.
/// All sub-headers e.g riff.header, riff.wave_header, format.header, data.header
/// are in big-endian byte order.
struct wav_header
{
	constexpr static const std::size_t spec_sz =
		riff_header::spec_sz + format_header::spec_sz + data_header::spec_sz;
	//------------------
	// RIFF Header
	//------------------
	riff_header riff;

	//------------------
	// Format Header
	//------------------
	format_header format;

	//------------------
	// Data Header
	//------------------
	data_header data;
};

static void convert_to_little_endian(wav_header& header)
{
	detail::endian_swap(header.riff.header);
	detail::endian_swap(header.riff.wave_header);
	detail::endian_swap(header.format.header);
	detail::endian_swap(header.data.header);
}

static bool read_header(wav_header& header, const std::uint8_t* data)
{
	size_t offset = 0;

	std::memcpy(&header.riff, data, riff_header::spec_sz);

	offset += riff_header::spec_sz;

	std::memcpy(&header.format, data + offset, format_header::spec_sz);

	// here we do not use the format_header::spec_sz since some wave formats
	// it is not
	// data header starts after.
	std::size_t offset_to_data = sizeof(header.format.header) + sizeof(header.format.fmt_chunk_size) +
								 size_t(header.format.fmt_chunk_size);

	if(offset_to_data < format_header::spec_sz)
	{
		return false;
	}
	offset += offset_to_data;

	std::memcpy(&header.data, data + offset, data_header::spec_sz);

	return false;
}

bool load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
						  std::string& err)
{
	if(!data)
	{
		err = "ERROR : No data to load from.";
		return false;
	}
	if(!data_size || data_size <= sizeof(wav_header))
	{
		err = "ERROR : No data to load from.";
		return false;
	}

	wav_header header;
	if(read_header(header, data))
	{
		err = "ERROR : Incorrect wav header";
		return false;
	}
	// According to the Cannonical WAVE file format
	// all sub headers are in big-endian so we convert them to little
	convert_to_little_endian(header);

	if(std::memcmp(header.riff.header, "RIFF", 4) != 0)
	{
		err = "ERROR : Bad RIFF header.";
		return false;
	}

	if(std::memcmp(header.riff.wave_header, "WAVE", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	if(std::memcmp(header.format.header, "fmt ", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	if(std::memcmp(header.data.header, "data", 4) != 0)
	{
		err = "ERROR: This file is not wav format!";
		return false;
	}

	result.info.sample_rate = std::uint32_t(header.format.sample_rate);
	result.info.duration = sound_info::seconds_t(sound_info::seconds_t::rep(header.data.data_bytes) /
												 sound_info::seconds_t::rep(header.format.byte_rate));

	result.data.resize(std::size_t(header.data.data_bytes));
	result.info.bytes_per_sample = std::uint8_t(header.format.bit_depth) / 8;

	std::memcpy(result.data.data(), data + wav_header::spec_sz, result.data.size());
	result.info.channels = std::uint32_t(header.format.num_channels);

	return true;
}
}

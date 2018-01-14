#include "utils.h"
#include "logger.h"
namespace audio
{
namespace utils
{
std::vector<std::uint8_t> convert_to_mono(const std::vector<std::uint8_t>& input,
										  std::uint8_t bytes_per_sample)
{
	if(bytes_per_sample > 2)
	{
		log_error("Sound buffer is not 8/16 bits per sample. Unsupported");
		return input;
	}

	std::vector<std::uint8_t> output;
	output.reserve(input.size() / 2);

	if(bytes_per_sample == 1)
	{
		for(std::size_t i = 0, sz = input.size(); i < sz; i += 2 * bytes_per_sample)
		{
			const std::uint8_t left = input[i];
			const std::uint8_t right = input[i + bytes_per_sample];
			const std::uint8_t mono_sample = std::uint8_t((int(left) + right) / 2);

			output.push_back(mono_sample);
		}
	}
	else if(bytes_per_sample == 2)
	{
		for(std::size_t i = 0, sz = input.size(); i < sz; i += 2 * bytes_per_sample)
		{
			const std::int16_t left = *reinterpret_cast<const std::int16_t*>(&input[i]);
			const std::int16_t right = *reinterpret_cast<const std::int16_t*>(&input[i + bytes_per_sample]);
			const std::int16_t mono_sample = std::int16_t((int(left) + right) / 2);

			const std::uint8_t sample_part1 = std::uint8_t(mono_sample >> 0);
			const std::uint8_t sample_part2 = std::uint8_t(mono_sample >> 8);
			output.push_back(sample_part1);
			output.push_back(sample_part2);
		}
	}
	return output;
}

std::vector<std::uint8_t> convert_to_stereo(const std::vector<std::uint8_t>& input,
											std::uint8_t bytes_per_sample)
{
	if(bytes_per_sample > 2)
	{
		log_error("Sound buffer is not 8/16 bits per sample");
		return input;
	}

	std::vector<std::uint8_t> output;
	output.resize(input.size() * 2, 0);

	for(std::size_t i = 0, sz = input.size(); i < sz; i += bytes_per_sample)
	{
		for(std::uint8_t bps = 0; bps < bytes_per_sample; ++bps)
		{
			const std::size_t src_idx = i + bps;
			const std::uint8_t mono_sample = input[src_idx];
			const std::size_t dst_idx = src_idx * 2;
			output[dst_idx + 0] = mono_sample;
			output[dst_idx + 1] = mono_sample;
		}
	}

	return output;
}
}
}

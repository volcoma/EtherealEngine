#include "sound_data.h"
#include "logger.h"
namespace audio
{
namespace utils
{
std::vector<std::uint8_t> convert_stereo_to_mono(const std::vector<std::uint8_t>& stereo)
{
	if(stereo.size() % 2 != 0)
	{
		log_error("Sound buffer is not complete");
		return stereo;
	}
	std::vector<std::uint8_t> mono;
	mono.reserve(stereo.size() / 2);

	for(size_t i = 0; i < stereo.size(); i += 2 * sizeof(int16_t))
	{
		int16_t left = *reinterpret_cast<const int16_t*>(&stereo[i]);
		int16_t right = *reinterpret_cast<const int16_t*>(&stereo[i + 2]);
		int16_t mono_sample = int16_t((int(left) + right) / 2);

		uint8_t sample_part1 = uint8_t(mono_sample >> 0);
		uint8_t sample_part2 = uint8_t(mono_sample >> 8);
		mono.push_back(sample_part1);
		mono.push_back(sample_part2);
	}
	return mono;
}

std::vector<std::uint8_t> convert_mono_to_stereo(const std::vector<std::uint8_t>& mono)
{
	if(mono.size() % 2 != 0)
	{
		log_error("Sound buffer is not complete");
		return mono;
	}
	std::vector<std::uint8_t> stereo;
	stereo.resize(mono.size() * 2, 0);

	for(size_t i = 0; i < mono.size(); i += sizeof(int16_t))
	{
		uint8_t sample_part1 = mono[i];
		uint8_t sample_part2 = mono[i + 1];

		size_t result_idx = i * 2;
		stereo[result_idx + 0] = sample_part1;
		stereo[result_idx + 1] = sample_part2;

		stereo[result_idx + 2] = sample_part1;
		stereo[result_idx + 3] = sample_part2;
	}
	return stereo;
}
}

double sound_data::get_duration_as_double() const
{
    return duration.count();
}

void sound_data::convert_to_mono()
{
    if(channels == 2)
    {
        data = utils::convert_stereo_to_mono(data);
        channels = 1;
    }
}

void sound_data::convert_to_stereo()
{
    if(channels == 1)
    {
        data = utils::convert_mono_to_stereo(data);
        channels = 1;
    }
}
}

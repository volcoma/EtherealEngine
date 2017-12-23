#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace audio
{

struct sound_data
{
	using duration_t = std::chrono::duration<double>;

    double get_duration_as_double() const { return duration.count();}

	std::vector<std::uint8_t> data;
	std::uint32_t sample_rate = 0;
	std::uint32_t channels = 0;
	duration_t duration = duration_t(0);
};
}

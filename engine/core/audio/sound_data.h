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

	std::vector<std::uint8_t> data;
	std::uint32_t sample_rate = 0;
	std::uint32_t channels = 0;
	duration_t duration = duration_t(0);
};
}

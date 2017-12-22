#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>

namespace audio
{

struct sound_data
{
    using duration_t = std::chrono::duration<double>;
    
    std::vector<std::int16_t> samples;
	std::uint32_t sample_rate = 0;
	std::uint32_t channels = 0;
	duration_t duration = duration_t(0);
};

}

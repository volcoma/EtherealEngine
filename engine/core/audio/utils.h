#pragma once

#include <cstdint>
#include <vector>

namespace audio
{

namespace utils
{
std::vector<std::uint8_t> convert_to_mono(const std::vector<std::uint8_t>& input,
										  std::uint8_t bytes_per_sample);
std::vector<std::uint8_t> convert_to_stereo(const std::vector<std::uint8_t>& input,
											std::uint8_t bytes_per_sample);
}
}

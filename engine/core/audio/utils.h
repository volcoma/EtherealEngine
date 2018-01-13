#pragma once

#include <cstdint>
#include <vector>

namespace audio
{

namespace utils
{

//-----------------------------------------------------------------------------
//  Name : convert_to_mono ()
/// <summary>
/// Converts an input buffer to mono
/// </summary>
//-----------------------------------------------------------------------------
std::vector<std::uint8_t> convert_to_mono(const std::vector<std::uint8_t>& input,
										  std::uint8_t bytes_per_sample);

//-----------------------------------------------------------------------------
//  Name : convert_to_stereo ()
/// <summary>
/// Converts an input buffer to stereo
/// </summary>
//-----------------------------------------------------------------------------
std::vector<std::uint8_t> convert_to_stereo(const std::vector<std::uint8_t>& input,
											std::uint8_t bytes_per_sample);
}
}

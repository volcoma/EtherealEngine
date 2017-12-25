#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace audio
{

namespace utils
{
std::vector<std::uint8_t> convert_stereo_to_mono(const std::vector<std::uint8_t>& stereo);
std::vector<std::uint8_t> convert_mono_to_stereo(const std::vector<std::uint8_t>& mono);
}

struct sound_data
{
	using duration_t = std::chrono::duration<double>;

	double get_duration_as_double() const;
	
	void convert_to_mono();
	void convert_to_stereo();

	/// data buffer of pcm sound int16_t per sample stored in uint8_t buffer
	std::vector<std::uint8_t> data;
    
    /// bytes per sample
    std::uint8_t bytes_per_sample = 2;

	/// sample rate of the sound
	std::uint32_t sample_rate = 0;

	/// channel count of the sound. e.g mono/stereo
	std::uint32_t channels = 0;

	/// duration of the sound in seconds
	duration_t duration = duration_t(0);
};
}

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

	duration_t::rep get_duration() const;
	
	//-----------------------------------------------------------------------------
	//  Name : convert_to_mono ()
	/// <summary>
	/// Converts internal data to mono/1 channel. Ideal for 3d positional sounds.
	/// </summary>
	//-----------------------------------------------------------------------------
	void convert_to_mono();
	
	//-----------------------------------------------------------------------------
	//  Name : convert_to_stereo ()
	/// <summary>
	/// Converts internal data to stereo/2 channels. These will not be affected by
	/// 3d attenuation.
	/// </summary>
	//-----------------------------------------------------------------------------
	void convert_to_stereo();

	/// data buffer of pcm sound stored in uint8_t buffer
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

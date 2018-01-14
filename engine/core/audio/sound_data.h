#pragma once

#include "sound_info.h"
#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace audio
{

struct sound_data
{
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

	/// info about the sound
	sound_info info;

	/// data buffer of pcm sound stored in uint8_t buffer
	std::vector<std::uint8_t> data;
};
}

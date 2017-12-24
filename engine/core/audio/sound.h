#pragma once

#include "sound_data.h"
#include <memory>

namespace audio
{
namespace priv
{
class sound_impl;
}

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : sound (Class)
/// <summary>
/// Storage for audio samples defining a sound.
/// </summary>
//-----------------------------------------------------------------------------
class sound
{
public:
	sound() = default;
	~sound();
	sound(sound_data&& data);
	sound(sound&& rhs);
	sound& operator=(sound&& rhs);

	sound(const sound& rhs) = delete;
	sound& operator=(const sound& rhs) = delete;

    //-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// Checks whether a sound is valid.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

    //-----------------------------------------------------------------------------
	//  Name : get_duration ()
	/// <summary>
	/// Gets the duration of the sound in seconds.
	/// </summary>
	//-----------------------------------------------------------------------------
	sound_data::duration_t get_duration() const;
	
	//-----------------------------------------------------------------------------
	//  Name : get_sample_rate ()
	/// <summary>
	/// Gets the sample rate of the sound.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_sample_rate() const;
	
	//-----------------------------------------------------------------------------
	//  Name : get_channels ()
	/// <summary>
	/// Gets the channels count of the sound. E.g mono/stereo
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_channels() const;
	
	//-----------------------------------------------------------------------------
	//  Name : get_data ()
	/// <summary>
	/// Gets the sound data rate of the sound.
	/// </summary>
	//-----------------------------------------------------------------------------
    const sound_data& get_data() const;

private:
    friend class source;

	sound_data _data;
	std::unique_ptr<priv::sound_impl> _impl;
};
}

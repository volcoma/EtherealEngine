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
	sound();
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
	//  Name : get_info ()
	/// <summary>
	/// Gets the sound data info.
	/// </summary>
	//-----------------------------------------------------------------------------
	const sound_info& get_info() const;

private:
	friend class source;

	/// pimpl idiom
	std::unique_ptr<priv::sound_impl> _impl;

	/// sound info
	sound_info _info;
};
}

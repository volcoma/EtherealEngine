#pragma once

#include "sound.h"

namespace audio
{
namespace priv
{
class source_impl;
}

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : source (Class)
/// <summary>
/// Emitter of a sound. It can control 3d spatial and emitter properties like
/// 3d position, 3d orientation, pitch, volume etc.
/// </summary>
//-----------------------------------------------------------------------------
class source
{
public:
	source();
	~source();
	source(source&& rhs);
	source& operator=(source&& rhs);

	source(const source& rhs) = delete;
	source& operator=(const source& rhs) = delete;

	//-----------------------------------------------------------------------------
	//  Name : bind ()
	/// <summary>
	/// Specifies the buffer to provide sound samples.
	/// </summary>
	//-----------------------------------------------------------------------------
	void bind(const sound& snd);

	//-----------------------------------------------------------------------------
	//  Name : set_loop ()
	/// <summary>
	/// Specifies whether source is looping.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_loop(bool on);

	//-----------------------------------------------------------------------------
	//  Name : set_volume ()
	/// <summary>
	/// A value of 1.0 means unattenuated. Each division by 2 equals an attenuation
	/// of about -6dB. Each multiplicaton by 2 equals an amplification of about +6dB.
	/// A value of 0.0 is meaningless with respect to a logarithmic scale; it is
	/// silent.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_volume(float volume);

	//-----------------------------------------------------------------------------
	//  Name : set_pitch ()
	/// <summary>
	/// A multiplier for the frequency (sample rate) of the source's buffer.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_pitch(float pitch);

	//-----------------------------------------------------------------------------
	//  Name : set_position ()
	/// <summary>
	/// The source location in three dimensional space.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_position(const float* position3);

	//-----------------------------------------------------------------------------
	//  Name : set_velocity ()
	/// <summary>
	/// Specifies the current velocity in local space.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_velocity(const float* velocity3);

	//-----------------------------------------------------------------------------
	//  Name : set_orientation ()
	/// <summary>
	/// Effectively two three dimensional vectors. The first vector is the front (or
	/// "at") and the second is the top (or "up").
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_orientation(const float* direction3, const float* up3);

	//-----------------------------------------------------------------------------
	//  Name : set_volume_rolloff ()
	/// <summary>
	/// Multiplier to exaggerate or diminish distance attenuation.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_volume_rolloff(float rolloff);

	//-----------------------------------------------------------------------------
	//  Name : set_distance ()
	/// <summary>
	/// Min = The distance in units that no attenuation occurs.
	/// Max = The distance above which the source is not attenuated any further with a
	/// clamped distance model, or where attenuation reaches 0.0 gain for linear
	/// distance models with a default rolloff factor.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_distance(float mind, float maxd);

	//-----------------------------------------------------------------------------
	//  Name : set_playing_offset ()
	/// <summary>
	/// Sets the source buffer position, in seconds.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_playing_offset(sound_data::duration_t offset);

	//-----------------------------------------------------------------------------
	//  Name : get_playing_offset ()
	/// <summary>
	/// Gets the source buffer position, in seconds.
	/// </summary>
	//-----------------------------------------------------------------------------
	sound_data::duration_t get_playing_offset() const;

	//-----------------------------------------------------------------------------
	//  Name : get_playing_duration ()
	/// <summary>
	/// Gets the source buffer length, in seconds.
	/// </summary>
	//-----------------------------------------------------------------------------
	sound_data::duration_t get_playing_duration() const;

	//-----------------------------------------------------------------------------
	//  Name : play ()
	/// <summary>
	/// Play, replay, or resume a Source.
	/// </summary>
	//-----------------------------------------------------------------------------
	void play();

	//-----------------------------------------------------------------------------
	//  Name : stop ()
	/// <summary>
	/// Stop a Source.
	///  </summary>
	//-----------------------------------------------------------------------------
	void stop();

	//-----------------------------------------------------------------------------
	//  Name : pause ()
	/// <summary>
	/// Pause a Source.
	/// </summary>
	//-----------------------------------------------------------------------------
	void pause();

	//-----------------------------------------------------------------------------
	//  Name : is_playing ()
	/// <summary>
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_playing() const;

	//-----------------------------------------------------------------------------
	//  Name : is_paused ()
	/// <summary>
	/// Checks whether a source is currently playing.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_paused() const;

	//-----------------------------------------------------------------------------
	//  Name : is_looping ()
	/// <summary>
	/// Checks whether a source is currently looping.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_looping() const;

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// Checks whether a source is valid.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

private:
	/// pimpl idiom
	std::unique_ptr<priv::source_impl> _impl;
};
}

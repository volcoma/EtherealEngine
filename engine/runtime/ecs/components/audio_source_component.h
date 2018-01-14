#pragma once

#include "../../assets/asset_handle.h"
#include "../ecs.h"
#include "core/audio/source.h"
#include "core/common/basetypes.hpp"
#include "core/math/math_includes.h"

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : audio_source_component (Class)
/// <summary>
/// Class that contains core data for audio listeners.
/// There can only be one instance of it per scene.
/// </summary>
//-----------------------------------------------------------------------------
class audio_source_component : public runtime::component_impl<audio_source_component>
{
	SERIALIZABLE(audio_source_component)
	REFLECTABLEV(audio_source_component, component)

public:
	//-------------------------------------------------------------------------
	// Public Virtual Methods (Override)

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : update ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void update(const math::transform& t);

	void set_loop(bool on);
	void set_volume(float volume);
	void set_pitch(float pitch);

	void set_volume_rolloff(float rolloff);
	void set_range(const frange& range);
	void set_autoplay(bool on);
	bool get_autoplay() const;

	float get_volume() const;
	float get_pitch() const;
	float get_volume_rolloff() const;
	const frange& get_range() const;

	void set_playing_offset(audio::sound_info::duration_t offset);
	audio::sound_info::duration_t get_playing_offset() const;
	audio::sound_info::duration_t get_playing_duration() const;

	void play();
	void stop();
	void pause();
	bool is_playing() const;
	bool is_paused() const;

	bool is_looping() const;

	void set_sound(asset_handle<audio::sound> sound);
	asset_handle<audio::sound> get_sound() const;

	bool has_binded_sound() const;

private:
	void apply_all();
	bool is_sound_valid() const;
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------

	bool _auto_play = true;
	bool _loop = true;
	float _volume = 1.0f;
	float _pitch = 1.0f;
	float _volume_rolloff = 1.0f;
	frange _range = {1.0f, 20.0f};
	audio::source _source;
	asset_handle<audio::sound> _sound;
};

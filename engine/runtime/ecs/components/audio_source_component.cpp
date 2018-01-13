#include "audio_source_component.h"
#include <limits>

void audio_source_component::update(const math::transform& t)
{
    auto pos = t.get_position();
    auto forward = t.z_unit_axis();
    auto up = t.y_unit_axis();
    _source.set_position({{pos.x, pos.y, pos.z}});
    _source.set_orientation({{forward.x, forward.y, forward.z}}, {{up.x, up.y, up.z}});
}

void audio_source_component::set_loop(bool on)
{
	_loop = on;
	_source.set_loop(on);
}

void audio_source_component::set_volume(float volume)
{
	math::clamp(volume, 0.0f, 1.0f);
	_volume = volume;
	_source.set_volume(volume);
}

void audio_source_component::set_pitch(float pitch)
{
	math::clamp(pitch, 0.5f, 2.0f);
	_pitch = pitch;
	_source.set_pitch(pitch);
}

void audio_source_component::set_volume_rolloff(float rolloff)
{
	math::clamp(rolloff, 0.0f, 10.0f);
	_volume_rolloff = rolloff;
	_source.set_volume_rolloff(rolloff);
}

void audio_source_component::set_range(const frange& range)
{
	math::clamp(range.min, 0.0f, range.max);
	math::clamp(range.max, range.min, std::numeric_limits<float>::max());

	_range = range;
	_source.set_distance(range.min, range.max);
}

void audio_source_component::set_autoplay(bool on)
{
	_auto_play = on;

	// Should this be here?
	if(_auto_play && !is_playing())
	{
		play();
	}
}

bool audio_source_component::get_autoplay() const
{
	return _auto_play;
}

float audio_source_component::get_volume() const
{
	return _volume;
}

float audio_source_component::get_pitch() const
{
	return _pitch;
}

float audio_source_component::get_volume_rolloff() const
{
	return _volume_rolloff;
}

const frange& audio_source_component::get_range() const
{
	return _range;
}

void audio_source_component::set_playing_offset(audio::sound_data::duration_t offset)
{
	_source.set_playing_offset(offset);
}

audio::sound_data::duration_t audio_source_component::get_playing_offset() const
{
	return _source.get_playing_offset();
}

audio::sound_data::duration_t audio_source_component::get_playing_duration() const
{
	return _source.get_playing_duration();
}

void audio_source_component::play()
{
	if(_sound)
	{
		_source.bind(*_sound.get());
		_source.play();
	}
}

void audio_source_component::stop()
{
	_source.stop();
}

void audio_source_component::pause()
{
	_source.pause();
}

bool audio_source_component::is_playing() const
{
	return _source.is_playing();
}

bool audio_source_component::is_paused() const
{
	return _source.is_paused();
}

bool audio_source_component::is_looping() const
{
	return _loop;
}

void audio_source_component::set_sound(asset_handle<audio::sound> sound)
{
	stop();

	_sound = sound;

	apply_all();
}

asset_handle<audio::sound> audio_source_component::get_sound() const
{
    return _sound;
}

bool audio_source_component::has_binded_sound() const
{
    return _source.has_binded_sound();
}

void audio_source_component::apply_all()
{
	set_loop(_loop);
	set_volume(_volume);
	set_pitch(_pitch);
	set_volume_rolloff(_volume_rolloff);
	set_range(_range);
	set_autoplay(_auto_play);
}

bool audio_source_component::is_sound_valid() const
{
	return _sound && _sound->is_valid();
}

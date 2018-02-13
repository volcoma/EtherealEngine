#include "audio_source_component.h"
#include <limits>

void audio_source_component::update(const math::transform& t)
{
	auto pos = t.get_position();
	auto forward = t.z_unit_axis();
	auto up = t.y_unit_axis();
	source_.set_position({{pos.x, pos.y, pos.z}});
	source_.set_orientation({{forward.x, forward.y, forward.z}}, {{up.x, up.y, up.z}});
}

void audio_source_component::set_loop(bool on)
{
	loop_ = on;
	source_.set_loop(on);
}

void audio_source_component::set_volume(float volume)
{
	math::clamp(volume, 0.0f, 1.0f);
	volume_ = volume;
	source_.set_volume(volume);
}

void audio_source_component::set_pitch(float pitch)
{
	math::clamp(pitch, 0.5f, 2.0f);
	pitch_ = pitch;
	source_.set_pitch(pitch);
}

void audio_source_component::set_volume_rolloff(float rolloff)
{
	math::clamp(rolloff, 0.0f, 10.0f);
	volume_rolloff_ = rolloff;
	source_.set_volume_rolloff(rolloff);
}

void audio_source_component::set_range(const frange_t& range)
{
	math::clamp(range.min, 0.0f, range.max);
	math::clamp(range.max, range.min, std::numeric_limits<float>::max());

	range_ = range;
	source_.set_distance(range.min, range.max);
}

void audio_source_component::set_autoplay(bool on)
{
	auto_play_ = on;

	// Should this be here?
	if(auto_play_ && !is_playing())
	{
		play();
	}
}

bool audio_source_component::get_autoplay() const
{
	return auto_play_;
}

float audio_source_component::get_volume() const
{
	return volume_;
}

float audio_source_component::get_pitch() const
{
	return pitch_;
}

float audio_source_component::get_volume_rolloff() const
{
	return volume_rolloff_;
}

const frange_t& audio_source_component::get_range() const
{
	return range_;
}

void audio_source_component::set_playing_offset(audio::sound_info::duration_t offset)
{
	source_.set_playing_offset(offset);
}

audio::sound_info::duration_t audio_source_component::get_playing_offset() const
{
	return source_.get_playing_offset();
}

audio::sound_info::duration_t audio_source_component::get_playing_duration() const
{
	return source_.get_playing_duration();
}

void audio_source_component::play()
{
	if(sound_)
	{
		source_.bind(*sound_.get());
		source_.play();
	}
}

void audio_source_component::stop()
{
	source_.stop();
}

void audio_source_component::pause()
{
	source_.pause();
}

bool audio_source_component::is_playing() const
{
	return source_.is_playing();
}

bool audio_source_component::is_paused() const
{
	return source_.is_paused();
}

bool audio_source_component::is_looping() const
{
	return loop_;
}

void audio_source_component::set_sound(asset_handle<audio::sound> sound)
{
	stop();

	sound_ = sound;

	apply_all();
}

asset_handle<audio::sound> audio_source_component::get_sound() const
{
	return sound_;
}

bool audio_source_component::has_binded_sound() const
{
	return source_.has_binded_sound();
}

void audio_source_component::apply_all()
{
	set_loop(loop_);
	set_volume(volume_);
	set_pitch(pitch_);
	set_volume_rolloff(volume_rolloff_);
	set_range(range_);
	set_autoplay(auto_play_);
}

bool audio_source_component::is_sound_valid() const
{
	return sound_ && sound_->is_valid();
}

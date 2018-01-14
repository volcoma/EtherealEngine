#include "source.h"
#include "impl/source_impl.h"

namespace audio
{
source::source()
	: _impl(std::make_unique<priv::source_impl>())
{
}

source::~source() = default;

source::source(source&& rhs)
	: _impl(std::move(rhs._impl))
{
	rhs._impl = nullptr;
}

source& source::operator=(source&& rhs)
{
	_impl = std::move(rhs._impl);
	rhs._impl = nullptr;

	return *this;
}

void source::play()
{
	if(is_valid())
	{
		_impl->play();
	}
}

void source::stop()
{
	if(is_valid())
	{
		_impl->stop();
	}
}

void source::pause()
{
	if(is_valid())
	{
		_impl->pause();
	}
}

bool source::is_playing() const
{
	if(is_valid())
	{
		return _impl->is_playing();
	}
	return false;
}

bool source::is_paused() const
{
	if(is_valid())
	{
		return _impl->is_paused();
	}
	return false;
}

bool source::is_stopped() const
{
	if(is_valid())
	{
		return _impl->is_stopped();
	}
	return true;
}

bool source::is_looping() const
{
	if(is_valid())
	{
		return _impl->is_looping();
	}
	return false;
}

void source::set_loop(bool on)
{
	if(is_valid())
	{
		_impl->set_loop(on);
	}
}

void source::set_volume(float volume)
{
	if(is_valid())
	{
		_impl->set_volume(volume);
	}
}

/* pitch, speed stretching */
void source::set_pitch(float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	if(is_valid())
	{
		_impl->set_pitch(pitch);
	}
}

void source::set_position(const float3& position)
{
	if(is_valid())
	{
		_impl->set_position(position);
	}
}

void source::set_velocity(const float3& velocity)
{
	if(is_valid())
	{
		_impl->set_velocity(velocity);
	}
}

void source::set_orientation(const float3& direction, const float3& up)
{
	if(is_valid())
	{
		_impl->set_orientation(direction, up);
	}
}

void source::set_volume_rolloff(float rolloff)
{
	if(is_valid())
	{
		_impl->set_volume_rolloff(rolloff);
	}
}

void source::set_distance(float mind, float maxd)
{
	if(is_valid())
	{
		_impl->set_distance(mind, maxd);
	}
}

void source::set_playing_offset(sound_info::duration_t offset)
{
	if(is_valid())
	{
		_impl->set_playing_offset(float(offset.count()));
	}
}

sound_info::duration_t source::get_playing_offset() const
{
	if(is_valid())
	{
		return sound_info::duration_t(_impl->get_playing_offset());
	}
	return sound_info::duration_t(0);
}

sound_info::duration_t source::get_playing_duration() const
{
	if(is_valid())
	{
		return sound_info::duration_t(_impl->get_playing_duration());
	}
	return sound_info::duration_t(0);
}

bool source::is_valid() const
{
	return _impl && _impl->is_valid();
}

void source::bind(const sound& snd)
{
	if(is_valid())
	{
		_impl->bind(snd._impl.get());
	}
}

bool source::has_binded_sound() const
{
	if(is_valid())
	{
		return _impl->has_binded_sound();
	}

	return false;
}
}

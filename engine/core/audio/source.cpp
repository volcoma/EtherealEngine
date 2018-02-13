#include "source.h"
#include "impl/source_impl.h"

namespace audio
{
source::source()
	: impl_(std::make_unique<priv::source_impl>())
{
}

source::~source() = default;

source::source(source&& rhs)
	: impl_(std::move(rhs.impl_))
{
	rhs.impl_ = nullptr;
}

source& source::operator=(source&& rhs)
{
	impl_ = std::move(rhs.impl_);
	rhs.impl_ = nullptr;

	return *this;
}

void source::play()
{
	if(is_valid())
	{
		impl_->play();
	}
}

void source::stop()
{
	if(is_valid())
	{
		impl_->stop();
	}
}

void source::pause()
{
	if(is_valid())
	{
		impl_->pause();
	}
}

bool source::is_playing() const
{
	if(is_valid())
	{
		return impl_->is_playing();
	}
	return false;
}

bool source::is_paused() const
{
	if(is_valid())
	{
		return impl_->is_paused();
	}
	return false;
}

bool source::is_stopped() const
{
	if(is_valid())
	{
		return impl_->is_stopped();
	}
	return true;
}

bool source::is_looping() const
{
	if(is_valid())
	{
		return impl_->is_looping();
	}
	return false;
}

void source::set_loop(bool on)
{
	if(is_valid())
	{
		impl_->set_loop(on);
	}
}

void source::set_volume(float volume)
{
	if(is_valid())
	{
		impl_->set_volume(volume);
	}
}

/* pitch, speed stretching */
void source::set_pitch(float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	if(is_valid())
	{
		impl_->set_pitch(pitch);
	}
}

void source::set_position(const float3& position)
{
	if(is_valid())
	{
		impl_->set_position(position);
	}
}

void source::set_velocity(const float3& velocity)
{
	if(is_valid())
	{
		impl_->set_velocity(velocity);
	}
}

void source::set_orientation(const float3& direction, const float3& up)
{
	if(is_valid())
	{
		impl_->set_orientation(direction, up);
	}
}

void source::set_volume_rolloff(float rolloff)
{
	if(is_valid())
	{
		impl_->set_volume_rolloff(rolloff);
	}
}

void source::set_distance(float mind, float maxd)
{
	if(is_valid())
	{
		impl_->set_distance(mind, maxd);
	}
}

void source::set_playing_offset(sound_info::duration_t offset)
{
	if(is_valid())
	{
		impl_->set_playing_offset(float(offset.count()));
	}
}

sound_info::duration_t source::get_playing_offset() const
{
	if(is_valid())
	{
		return sound_info::duration_t(impl_->get_playing_offset());
	}
	return sound_info::duration_t(0);
}

sound_info::duration_t source::get_playing_duration() const
{
	if(is_valid())
	{
		return sound_info::duration_t(impl_->get_playing_duration());
	}
	return sound_info::duration_t(0);
}

bool source::is_valid() const
{
	return impl_ && impl_->is_valid();
}

void source::bind(const sound& snd)
{
	if(is_valid())
	{
		impl_->bind(snd.impl_.get());
	}
}

bool source::has_binded_sound() const
{
	if(is_valid())
	{
		return impl_->has_binded_sound();
	}

	return false;
}
}

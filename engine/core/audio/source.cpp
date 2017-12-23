#include "source.h"
#include "logger.h"
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
	if(_impl)
	{
		_impl->play();
	}
}

void source::stop()
{
	if(_impl)
	{
		_impl->stop();
	}
}

void source::pause()
{
	if(_impl)
	{
		_impl->pause();
	}
}

bool source::is_playing() const
{
	return _impl && _impl->is_playing();
}

bool source::is_looping() const
{
	if(_impl)
	{
		return _impl->is_looping();
	}
	return false;
}

void source::set_loop(bool on)
{
	if(_impl)
	{
		_impl->set_loop(on);
	}
}

void source::set_gain(float gain)
{
	if(_impl)
	{
		_impl->set_gain(gain);
	}
}

/* pitch, speed stretching */
void source::set_pitch(float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	if(_impl)
	{
		_impl->set_pitch(pitch);
	}
}

void source::set_position(const float* position3)
{
	if(_impl)
	{
		_impl->set_position(position3);
	}
}

void source::set_velocity(const float* velocity3)
{
	if(_impl)
	{
		_impl->set_velocity(velocity3);
	}
}

void source::set_direction(const float* direction3)
{
	if(_impl)
	{
		_impl->set_direction(direction3);
	}
}

void source::set_attenuation(float roll_off)
{
	if(_impl)
	{
		_impl->set_attenuation(roll_off);
	}
}

void source::set_distance(float mind, float maxd)
{
	if(_impl)
	{
		_impl->set_distance(mind, maxd);
	}
}

void source::set_playing_offset(sound_data::duration_t offset)
{
	if(_impl)
	{
		_impl->set_playing_offset(float(offset.count()));
	}
}

bool source::is_valid() const
{
	return _impl && _impl->is_valid();
}

void source::bind(const sound& snd)
{
	if(is_valid())
	{
        if(snd.get_channels() > 1)
        {
            log("Sound is not mono. 3D Attenuation will not work.");
        }
		_impl->bind(snd._impl->native_handle());
	}
}
}

#include "source.h"
#include "impl/source_impl.h"

namespace audio
{
source::source()
{
    _impl = std::make_unique<source_impl>();
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

void source::loop(bool on)
{
	if(_impl)
	{
		_impl->loop(on);
	}
}

void source::gain(float gain)
{
	if(_impl)
	{
		_impl->gain(gain);
	}
}

/* pitch, speed stretching */
void source::pitch(float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	if(_impl)
	{
		_impl->pitch(pitch);
	}
}

void source::position(const float* position3, bool relative)
{
	if(_impl)
	{
		_impl->position(position3, relative);
	}
}

void source::velocity(const float* velocity3)
{
	if(_impl)
	{
		_impl->velocity(velocity3);
	}
}

void source::direction(const float* direction3)
{
	if(_impl)
	{
		_impl->direction(direction3);
	}
}

void source::attenuation(float roll_off, float ref_distance)
{
	if(_impl)
    {
        _impl->attenuation(roll_off, ref_distance);
    }
}

void source::distance(float mind, float maxd)
{
	if(_impl)
    {
        _impl->distance(mind, maxd);
    }
    }

bool source::is_valid() const
{
	return _impl && _impl->is_valid();
}

void source::play(const sound& snd)
{
    if(is_valid())
    {
        _impl->bind(snd.get_impl()->native_handle());
        _impl->play();
    }
}

}

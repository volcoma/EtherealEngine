#include "listener.h"
#include "impl/listener_impl.h"

namespace audio
{
listener::listener()
{
	_impl = std::make_unique<listener_impl>();
	gain(1.0f);
}

void listener::gain(const float gain)
{
	if(_impl)
	{
		_impl->gain(gain);
	}
}

void listener::position(const float* position3)
{
	if(_impl)
	{
		_impl->position(position3);
	}
}

void listener::velocity(const float* velocity3)
{
	if(_impl)
	{
		_impl->velocity(velocity3);
	}
}

void listener::direction(const float* direction3)
{
	if(_impl)
	{
		_impl->direction(direction3);
	}
}
}

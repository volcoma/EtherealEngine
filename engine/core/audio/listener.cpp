#include "listener.h"
#include "impl/listener_impl.h"

namespace audio
{
listener::listener()
	: _impl(std::make_unique<priv::listener_impl>())
{
    set_gain(1.0f);
}

listener::~listener() = default;
    
void listener::set_gain(const float gain)
{
	if(_impl)
	{
		_impl->set_gain(gain);
	}
}

void listener::set_position(const float* position3)
{
	if(_impl)
	{
		_impl->set_position(position3);
	}
}

void listener::set_velocity(const float* velocity3)
{
	if(_impl)
	{
		_impl->set_velocity(velocity3);
	}
}

void listener::set_direction(const float* direction3)
{
	if(_impl)
	{
		_impl->set_direction(direction3);
	}
}
}
